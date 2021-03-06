/*
 * Copyright (C) 2010-2011 Fabrício Godoy <skarllot@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Fabrício Godoy <skarllot@gmail.com>
 *
 */

#include "chatserver.h"
#include "common/list.h"
#include "common/pchar.h"

#include <config.h>
#include <pthread.h>
#include <stdio.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#else
#include "windef.h"
#endif

#define DEFAULT_PORT 1100
#define BUFFER_SIZE 1024
#define NICK_LEN_MIN 5
#define WRITE_FLUSH_INTERVAL 100

#define BREAK_LINE "\n\r"

#define CMD_NO_CODE -1
#define CMD_EXIT "EXIT"
#define CMD_PASS "PASS"
#define CMD_NICK "NICK"
#define CMD_CHAT_LIST "LIST"
#define CMD_CHAT_BROAD "BROAD"

#define MSG_WELCOME "!OK Connected (" PACKAGE_STRING ")"
#define MSG_INVALID "!ERROR Invalid command"
#define MSG_PASS_FAIL "!ERROR Wrong password"
#define MSG_PASS_OK "!OK Password accepted"
#define MSG_NICK_FAIL "!ERROR Invalid nick"
#define MSG_NICK_OK "!OK Nick accepted"
#define MSG_CHAT_LIST_OK "!OK List comming"
#define MSG_CHAT_BROAD_FAIL "!ERROR Empty message"
#define MSG_CHAT_BROAD_OK "!OK Broading message to all"
#define MSG_END ".\n"

#define LEVEL_EXITING -1
#define LEVEL_PASS 0
#define LEVEL_NICK 1
#define LEVEL_CHAT 2

struct _chatserver_it
{
    int serverfd;
    BOOLEAN stopping;
    list_t *clientwa_list;
    // Locks concurrent access to clientwa_list
    pthread_mutex_t m_cli_list;
    char *pass;
};

typedef struct
{
    chatserver_it *parent;
    int fd;
    // List of messages from client to server
    pchar_ll_t *read_list;
    // Handles multi-line commands
    int last_cmd;
    // List of messages from server to client
    pchar_ll_t *write_list;
    // Locks concurrent access to write_list
    pthread_mutex_t m_wrlist;
    int level;
    char *nick;
    BOOLEAN exit;
} clientwa_t;

static void chatserver_acceptclients(chatserver_t *);
static void *chatserver_clienttalk(void *context);
static int flush_level_global(clientwa_t *cliwa, const char *cmd, const char *param);
static int flush_level_pass(clientwa_t *cliwa, const char *cmd, const char *param);
static int flush_level_nick(clientwa_t *cliwa, const char *cmd, char *param);
static int flush_level_chat(clientwa_t *cliwa, const char *cmd, const char *param);
static void flush_rdlist(clientwa_t *cliwa);
static void *flush_wrlist(void *context);

chatserver_t *chatserver_create()
{
    chatserver_t *csrv;
    MALLOC(csrv, chatserver_t);

    MALLOC(csrv->priv, chatserver_it);
    csrv->priv->serverfd = -1;
    csrv->priv->stopping = FALSE;
    csrv->priv->clientwa_list = list_create(-1);
    csrv->priv->pass = NULL;

    if (pthread_mutex_init(&csrv->priv->m_cli_list, NULL) != 0) {
        perror("Cannot initializate m_cli_list mutex");
        exit(EXIT_FAILURE);
    }

    return csrv;
}

void chatserver_free(chatserver_t *csrv)
{
    // Prevents undefined behavior
    if (csrv->priv->stopping == FALSE) {
        perror("Cannot free chatserver_t running instance");
        return;
    }

    pthread_mutex_lock(&csrv->priv->m_cli_list);
    int count = list_getcount(csrv->priv->clientwa_list);
    int i;
    for (i = count - 1; i >= 0; --i) {
        clientwa_t *item = (clientwa_t *)list_remove_at(
            csrv->priv->clientwa_list, i);

        if (item->fd >= 0)
            close(item->fd);
        pchar_ll_free(item->read_list);
        pchar_ll_free(item->write_list);
        free(item);
    }

    list_free(csrv->priv->clientwa_list);
    pthread_mutex_unlock(&csrv->priv->m_cli_list);
    if (pthread_mutex_destroy(&csrv->priv->m_cli_list) != 0) {
        perror("Cannot destroy m_cli_list mutex");
        exit(EXIT_FAILURE);
    }

    free(csrv->priv);

    free(csrv);
}

void chatserver_load(chatserver_t *csrv)
{
    csrv->priv->serverfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (csrv->priv->serverfd == -1) {
        perror("Cannot create server socket");
        exit(EXIT_FAILURE);
    }

// Allow reuse an already closed socket (not reliable)
#ifdef __linux__
    int allowreuse = 1;
#elif __sun__
    char allowreuse = '1';
#endif
    if (setsockopt(csrv->priv->serverfd, SOL_SOCKET, SO_REUSEADDR, &allowreuse,
                   sizeof(allowreuse)) == -1) {
        perror("Cannot configure server socket");
        close(csrv->priv->serverfd);
        exit(EXIT_FAILURE);
    }

    char *buf = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    memset(buf, 0, sizeof(char) * BUFFER_SIZE);

    printf("Type server password (empty to none): ");
    fgets(buf, BUFFER_SIZE, stdin);
    pchar_remove_lbreaks(buf);
    int len = strlen(buf);
    if (len > 0) {
        csrv->priv->pass = pchar_substring(buf, 0, len);
    }
}

void chatserver_start(chatserver_t *csrv)
{
    struct sockaddr_in myaddr;
    
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;                    // IPv4
    myaddr.sin_port = htons(DEFAULT_PORT);          // 0x0b (x86) -> 0xb0 (inet)
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);     // Listen all interfaces

    // Set address to socket
    if (bind(csrv->priv->serverfd, (const struct sockaddr *)&myaddr,
             sizeof(struct sockaddr_in)) == -1) {
        perror("Cannot bind address to server socket");
        close(csrv->priv->serverfd);
        exit(EXIT_FAILURE);
    }

    // Open socket for clients awaiting
    if (listen(csrv->priv->serverfd, 10) == -1) {
        perror("Cannot listen for connection using server socket");
        close(csrv->priv->serverfd);
        exit(EXIT_FAILURE);
    }

    printf("Awaiting for client connections...\n");
    chatserver_acceptclients(csrv);
}

void chatserver_stop(chatserver_t *csrv)
{
    csrv->priv->stopping = TRUE;
    shutdown(csrv->priv->serverfd, SHUT_RDWR);
    close(csrv->priv->serverfd);
}

void chatserver_acceptclients(chatserver_t *csrv)
{
    for (;;) {
        clientwa_t *cliwa = NULL;
        MALLOC(cliwa, clientwa_t);
        cliwa->parent = csrv->priv;

        cliwa->fd = accept(csrv->priv->serverfd, NULL, NULL);
        if (csrv->priv->stopping == TRUE && cliwa->fd < 0) {
            break;
        }
        if (cliwa->fd < 0) {
            perror("Error accepting client connection");
            close(csrv->priv->serverfd);
            exit(EXIT_FAILURE);
        }
        printf("Client %d connected.\n", cliwa->fd);

        pthread_mutex_lock(&csrv->priv->m_cli_list);
        list_add(csrv->priv->clientwa_list, cliwa);
        pthread_mutex_unlock(&csrv->priv->m_cli_list);

        pthread_t thread;
        pthread_create(&thread, NULL, chatserver_clienttalk, cliwa);
    }
}

void *chatserver_clienttalk(void *context)
{
    clientwa_t *cliwa = (clientwa_t *)context;
    cliwa->exit = FALSE;
    MALLOC(cliwa->read_list, pchar_ll_t);
    cliwa->last_cmd = CMD_NO_CODE;
    MALLOC(cliwa->write_list, pchar_ll_t);
    cliwa->level = cliwa->parent->pass ? LEVEL_PASS : LEVEL_NICK;
    
    if (pthread_mutex_init(&cliwa->m_wrlist, NULL) != 0) {
        perror("Cannot initializate m_wrlist mutex");
        exit(EXIT_FAILURE);
    }

    char *climsg = (char *)malloc(BUFFER_SIZE);
    pchar_ll_append(cliwa->write_list, MSG_WELCOME);
    
    pthread_t th_wrlist;
    pthread_create(&th_wrlist, NULL, flush_wrlist, cliwa);

    while (!cliwa->exit) {
        memset(climsg, 0, BUFFER_SIZE);
        if (read(cliwa->fd, climsg, BUFFER_SIZE - 1) < 0) {
            perror("Cannot receive message from client");
            break;
        }

        if (strlen(climsg) > 0)
            cliwa->read_list->next = pchar_split(climsg, BREAK_LINE);

        flush_rdlist(cliwa);
    }
    
    pthread_mutex_lock(&cliwa->m_wrlist);
    cliwa->level = LEVEL_EXITING;
    pthread_mutex_unlock(&cliwa->m_wrlist);

    shutdown(cliwa->fd, SHUT_RDWR);
    close(cliwa->fd);
    printf("Client %d disconnected.\n", cliwa->fd);

    pthread_mutex_lock(&cliwa->parent->m_cli_list);
    list_remove(cliwa->parent->clientwa_list, cliwa);
    pthread_mutex_unlock(&cliwa->parent->m_cli_list);
        
    pthread_mutex_destroy(&cliwa->m_wrlist);
    cliwa->parent = NULL;
    pchar_ll_free(cliwa->read_list);
    pchar_ll_free(cliwa->write_list);
    free(cliwa);

    pthread_exit(NULL);
}

int flush_level_global(clientwa_t *cliwa, const char *cmd, const char *param)
{
    if (strcmp(cmd, CMD_EXIT) == 0) {
        cliwa->exit = TRUE;
        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

int flush_level_pass(clientwa_t *cliwa, const char *cmd, const char *param)
{
    if (strcmp(cmd, CMD_PASS) == 0) {
        pthread_mutex_lock(&cliwa->m_wrlist);
        
        if (param && strcmp(param, cliwa->parent->pass) == 0) {
            cliwa->level++;
            pchar_ll_append(cliwa->write_list, MSG_PASS_OK);
        }
        else {
            pchar_ll_append(cliwa->write_list, MSG_PASS_FAIL);
        }
        
        pthread_mutex_unlock(&cliwa->m_wrlist);
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

int flush_level_nick(clientwa_t *cliwa, const char *cmd, char *param)
{
    if (strcmp(cmd, CMD_NICK) == 0) {
        if (!param) {
            pthread_mutex_lock(&cliwa->m_wrlist);
            pchar_ll_append(cliwa->write_list, MSG_NICK_FAIL);
            pthread_mutex_unlock(&cliwa->m_wrlist);
            
            return EXIT_SUCCESS;
        }
        
        pchar_trim_spaces(param);
        if (strlen(param) < NICK_LEN_MIN) {
            pthread_mutex_lock(&cliwa->m_wrlist);
            pchar_ll_append(cliwa->write_list, MSG_NICK_FAIL);
            pthread_mutex_unlock(&cliwa->m_wrlist);
            
            return EXIT_SUCCESS;
        }
        
        char *newnick = pchar_copy(param);
        pchar_tolower(newnick);
        list_t *clilist = cliwa->parent->clientwa_list;
        
        pthread_mutex_lock(&cliwa->parent->m_cli_list);
        BOOLEAN valid = TRUE;
        int i, count = list_getcount(clilist);
        
        for (i = 0; i < count; i++) {
            char *c_nick = ((clientwa_t *)list_get(clilist, i))->nick;
            if (!c_nick)
                continue;
            
            c_nick = pchar_copy(c_nick);
            pchar_tolower(c_nick);
            if (strcmp(newnick, c_nick) == 0) {
                valid = FALSE;
                break;
            }
        }
        pthread_mutex_unlock(&cliwa->parent->m_cli_list);
        
        free(newnick);
        
        pthread_mutex_lock(&cliwa->m_wrlist);
        
        if (valid) {
            cliwa->nick = pchar_copy(param);
            printf("New nick \"%s\" from client %d\n", cliwa->nick, cliwa->fd);
            pchar_ll_append(cliwa->write_list, MSG_NICK_OK);
            cliwa->level++;
        }
        else
            pchar_ll_append(cliwa->write_list, MSG_NICK_FAIL);
        
        pthread_mutex_unlock(&cliwa->m_wrlist);
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

int flush_level_chat(clientwa_t *cliwa, const char *cmd, const char *param)
{
    if (strcmp(cmd, CMD_CHAT_LIST) == 0) {
        pthread_mutex_lock(&cliwa->m_wrlist);
        
        pchar_ll_append(cliwa->write_list, MSG_CHAT_LIST_OK);
        
        list_t *clilist = cliwa->parent->clientwa_list;
        
        pthread_mutex_lock(&cliwa->parent->m_cli_list);
        int i, count = list_getcount(clilist);
        for (i = 0; i < count; i++) {
            clientwa_t *c_cliwa = (clientwa_t *)list_get(clilist, i);
            
            if (c_cliwa == cliwa || c_cliwa->level != LEVEL_CHAT)
                continue;
            
            pchar_ll_append(cliwa->write_list, c_cliwa->nick);
        }
        pthread_mutex_unlock(&cliwa->parent->m_cli_list);
        
        pchar_ll_append(cliwa->write_list, MSG_END);
        
        pthread_mutex_unlock(&cliwa->m_wrlist);
        return EXIT_SUCCESS;
    }
    else if (strcmp(cmd, CMD_CHAT_BROAD) == 0) {
        if (!param || strlen(param) < 1) {
            pthread_mutex_lock(&cliwa->m_wrlist);
            pchar_ll_append(cliwa->write_list, MSG_CHAT_BROAD_FAIL);
            pthread_mutex_unlock(&cliwa->m_wrlist);
            
            return EXIT_SUCCESS;
        }
        
        pthread_mutex_lock(&cliwa->m_wrlist);
        pchar_ll_append(cliwa->write_list, MSG_CHAT_BROAD_OK);
        pthread_mutex_unlock(&cliwa->m_wrlist);
        
        list_t *clilist = cliwa->parent->clientwa_list;
        char *msgheader = pchar_concat_3(CMD_CHAT_BROAD, " ", cliwa->nick);
        
        pthread_mutex_lock(&cliwa->parent->m_cli_list);
        int i, count = list_getcount(clilist);
        for (i = 0; i < count; i++) {
            clientwa_t *c_cliwa = (clientwa_t *)list_get(clilist, i);
            
            if (c_cliwa == cliwa)
                continue;
            
            if (pthread_mutex_lock(&c_cliwa->m_wrlist) == 0) {
                if (c_cliwa->level != LEVEL_CHAT) {
                    pthread_mutex_unlock(&c_cliwa->m_wrlist);
                    continue;
                }

                pchar_ll_append(c_cliwa->write_list, msgheader);
                pchar_ll_append(c_cliwa->write_list, param);
                pchar_ll_append(c_cliwa->write_list, MSG_END);
                pthread_mutex_unlock(&c_cliwa->m_wrlist);
            }
        }
        pthread_mutex_unlock(&cliwa->parent->m_cli_list);
        
        free(msgheader);
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

void flush_rdlist(clientwa_t *cliwa)
{
    pchar_ll_t *curr = cliwa->read_list->next;
    int idx, ret;
    char *cmd, *param;

    while (curr) {
        printf("< %d:%s\n", cliwa->fd, curr->node);

        cmd = curr->node;
        param = NULL;
        idx = pchar_index_of(cmd, ' ');
        if (idx > 0) {
            cmd[idx] = '\0';
            param = &cmd[idx + 1];
        }

        ret = EXIT_FAILURE;
        switch (cliwa->level) {
            case LEVEL_PASS:
                ret = flush_level_pass(cliwa, cmd, param);
                break;
            case LEVEL_NICK:
                ret = flush_level_nick(cliwa, cmd, param);
                break;
            case LEVEL_CHAT:
                ret = flush_level_chat(cliwa, cmd, param);
                break;
        }

        if (ret == EXIT_FAILURE) {
            if (flush_level_global(cliwa, cmd, param) == EXIT_FAILURE) {
                pthread_mutex_lock(&cliwa->m_wrlist);
                pchar_ll_append(cliwa->write_list, MSG_INVALID);
                pthread_mutex_unlock(&cliwa->m_wrlist);
            }
        }

        curr = curr->next;
    }

    pchar_ll_free(cliwa->read_list->next);
    cliwa->read_list->next = NULL;
}

void *flush_wrlist(void *context)
{
    clientwa_t *cliwa = (clientwa_t *)context;
    
    while (cliwa->level != LEVEL_EXITING) {
        pthread_mutex_lock(&cliwa->m_wrlist);
        pchar_ll_t *curr = cliwa->write_list->next;

        while (curr) {
            printf("> %d:%s\n", cliwa->fd, curr->node);

            if (write(cliwa->fd, curr->node, strlen(curr->node)) < 0) {
                perror("Cannot send message to client");
                break;
            }
            if (write(cliwa->fd, "\n", strlen("\n")) < 0) {
                perror("Cannot send message to client");
                break;
            }

            curr = curr->next;
        }

        pchar_ll_free(cliwa->write_list->next);
        cliwa->write_list->next = NULL;
        pthread_mutex_unlock(&cliwa->m_wrlist);
        usleep(WRITE_FLUSH_INTERVAL);
    }
    
    pthread_exit(NULL);
}
