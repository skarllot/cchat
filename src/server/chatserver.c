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

#include <pthread.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 1100

#define COMMANDS_EXIT "EXIT\r\n"

struct _chatserver_it
{
    int serverfd;
    BOOLEAN stopping;
    list_t *clientfd_list;
};

typedef struct
{
    chatserver_t *instance;
    int clientindex;
} thcontext_t;

static void chatserver_acceptclients(chatserver_t *);
static void *chatserver_clienttalk(void *context);

chatserver_t *chatserver_create()
{
    chatserver_t *csrv;
    MALLOC(csrv, chatserver_t);

    MALLOC(csrv->priv, chatserver_it);
    csrv->priv->serverfd = -1;
    csrv->priv->stopping = FALSE;
    csrv->priv->clientfd_list = list_create(-1);
    return csrv;
}

void chatserver_free(chatserver_t *csrv)
{
    int count = list_getcount(csrv->priv->clientfd_list);
    int i;
    for (i = count - 1; i >= 0; --i) {
        int *item = (int *)list_remove(csrv->priv->clientfd_list, i);
        free(item);
    }

    list_free(csrv->priv->clientfd_list);
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

    int allowreuse = 1;
    if (setsockopt(csrv->priv->serverfd, SOL_SOCKET, SO_REUSEADDR, &allowreuse,
                   sizeof(allowreuse)) == -1) {
        perror("Cannot manipulate server socket");
        close(csrv->priv->serverfd);
        exit(EXIT_FAILURE);
    }
}

void chatserver_start(chatserver_t *csrv)
{
    struct sockaddr_in myaddr;
    
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(DEFAULT_PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(csrv->priv->serverfd, (const struct sockaddr *)&myaddr,
             sizeof(struct sockaddr_in)) == -1) {
        perror("Cannot bind address to server socket");
        close(csrv->priv->serverfd);
        exit(EXIT_FAILURE);
    }

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
        int *clientfd = (int *)malloc(sizeof(int));
        *clientfd = accept(csrv->priv->serverfd, NULL, NULL);
        if (csrv->priv->stopping == TRUE && *clientfd < 0) {
            break;
        }
        if (*clientfd < 0) {
            perror("Error accepting client connection");
            close(csrv->priv->serverfd);
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        list_add(csrv->priv->clientfd_list, clientfd);
        thcontext_t *context;
        MALLOC(context, thcontext_t);
        context->instance = csrv;
        context->clientindex = list_getcount(csrv->priv->clientfd_list) - 1;

        pthread_t thread;
        pthread_create(&thread, NULL, chatserver_clienttalk, context);
    }
}

void *chatserver_clienttalk(void *context)
{
    thcontext_t *tcontext = (thcontext_t *)context;
    int clientfd = *(int *)list_get(tcontext->instance->priv->clientfd_list,
                                    tcontext->clientindex);

    while (TRUE) {
        char mymsg[] = "Command: ";
        if (write(clientfd, mymsg, strlen(mymsg)) < 0) {
            perror("Cannot send message to client");
            break;
        }

        char climsg[256];
        memset(climsg, 0, 256);
        if (read(clientfd, climsg, 255) < 0) {
            perror("Cannot receive message from client");
            break;
        }

        if (strcmp(climsg, COMMANDS_EXIT) == 0) {
            break;
        }
        printf("Client message: %s\n", climsg);
    }

    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);
    printf("Client disconnected.\n");

    free(context);
    pthread_exit(NULL);
}

