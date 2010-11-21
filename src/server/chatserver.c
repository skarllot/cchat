/*
 * Copyright (C) 2010 Fabrício Godoy <skarllot@gmail.com>
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
#else
#include "windef.h"
#endif

#define DEFAULT_PORT 1100
#define BUFFER_SIZE 1024

#define BREAK_LINE "\n\r"

#define CMD_NO_CODE -1
#define CMD_EXIT "EXIT"
#define CMD_PASS "PASS"
//#define CMD_PASS_CODE 0

#define MSG_WELCOME "!OK Connected (" PACKAGE_STRING ")"
#define MSG_INVALID "!ERROR Invalid command"
#define MSG_PASS_FAIL "!ERROR Wrong password"
#define MSG_PASS_OK "!OK Password accepted"
#define MSG_OK "OK"
#define MSG_FAIL "FAIL"
#define MSG_PASS "PASS?"
#define MSG_NICK "NICK?"

#define LEVEL_PASS 0
#define LEVEL_NICK 1
#define LEVEL_MSG 2

struct _chatserver_it
{
    int serverfd;
    BOOLEAN stopping;
    list_t *clientwa_list;
    char *pass;
};

typedef struct
{
    chatserver_it *parent;
    int fd;
    pchar_ll_t *cmd_list;
    int last_cmd;
    pchar_ll_t *msg_list;
    BOOLEAN exit;
    int level;
} clientwa_t;

static void chatserver_acceptclients(chatserver_t *);
static void *chatserver_clienttalk(void *context);
static void flush_commands(clientwa_t *cliwa);
static void flush_messages(clientwa_t *cliwa);

chatserver_t *chatserver_create()
{
    chatserver_t *csrv;
    MALLOC(csrv, chatserver_t);

    MALLOC(csrv->priv, chatserver_it);
    csrv->priv->serverfd = -1;
    csrv->priv->stopping = FALSE;
    csrv->priv->clientwa_list = list_create(-1);
    csrv->priv->pass = NULL;
    return csrv;
}

void chatserver_free(chatserver_t *csrv)
{
    int count = list_getcount(csrv->priv->clientwa_list);
    int i;
    for (i = count - 1; i >= 0; --i) {
        clientwa_t *item = (clientwa_t *)list_remove(
            csrv->priv->clientwa_list, i);

        if (item->fd >= 0)
            close(item->fd);
        pchar_ll_free(item->cmd_list);
        pchar_ll_free(item->msg_list);
        free(item);
    }

    list_free(csrv->priv->clientwa_list);
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
        perror("Cannot manipulate server socket");
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

        list_add(csrv->priv->clientwa_list, cliwa);

        pthread_t thread;
        pthread_create(&thread, NULL, chatserver_clienttalk, cliwa);
    }
}

void *chatserver_clienttalk(void *context)
{
    clientwa_t *cliwa = (clientwa_t *)context;
    cliwa->exit = FALSE;
    MALLOC(cliwa->cmd_list, pchar_ll_t);
    cliwa->last_cmd = CMD_NO_CODE;
    MALLOC(cliwa->msg_list, pchar_ll_t);
    cliwa->level = cliwa->parent->pass ? LEVEL_PASS : LEVEL_NICK;

    char *climsg = (char *)malloc(BUFFER_SIZE);
    pchar_ll_append(cliwa->msg_list, MSG_WELCOME);

    while (!cliwa->exit) {
        flush_messages(cliwa);

        memset(climsg, 0, BUFFER_SIZE);
        if (read(cliwa->fd, climsg, BUFFER_SIZE - 1) < 0) {
            perror("Cannot receive message from client");
            break;
        }

        if (strlen(climsg) > 0)
            cliwa->cmd_list->next = pchar_split(climsg, BREAK_LINE);

        flush_commands(cliwa);
    }

    shutdown(cliwa->fd, SHUT_RDWR);
    close(cliwa->fd);
    printf("Client %d disconnected.\n", cliwa->fd);

    // TODO: Remove from list
    cliwa->fd = -1;
    //free(context);
    pthread_exit(NULL);
}

void flush_commands(clientwa_t *cliwa)
{
    pchar_ll_t *curr = cliwa->cmd_list->next;
    int idx;
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

        switch (cliwa->last_cmd) {
            case CMD_NO_CODE:
                switch (cliwa->level) {
                    case LEVEL_PASS:
                        if (strcmp(cmd, CMD_PASS) == 0 && param) {
                            if (strcmp(param, cliwa->parent->pass) == 0) {
                                cliwa->level++;
                                pchar_ll_append(cliwa->msg_list, MSG_PASS_OK);
                            }
                            else {
                                pchar_ll_append(cliwa->msg_list, MSG_PASS_FAIL);
                            }
                        }
                        else
                            pchar_ll_append(cliwa->msg_list, MSG_INVALID);

                        break;
                }

                if (strcmp(curr->node, CMD_EXIT) == 0) {
                    cliwa->exit = TRUE;
                }
                break;

            /*case CMD_PASS_CODE:
                
                break;*/
        }

        curr = curr->next;
    }

    pchar_ll_free(cliwa->cmd_list->next);
    cliwa->cmd_list->next = NULL;
}

void flush_messages(clientwa_t *cliwa)
{
    pchar_ll_t *curr = cliwa->msg_list->next;

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

    pchar_ll_free(cliwa->msg_list->next);
    cliwa->msg_list->next = NULL;
}

