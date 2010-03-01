/*
 * Copyright (C) 2009 Fabrício Godoy <skarllot@gmail.com>
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
#include "common/arraylist.h"

#include <pthread.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 1100

#define COMMANDS_EXIT "EXIT\r\n"

struct _ChatServer_private
{
    int serverfd;
    ArrayList *clientfd_list;
};

typedef struct
{
    ChatServer *instance;
    int clientindex;
} ThreadContext;

static void ChatServer_acceptclients(ChatServer *);
static void *ChatServer_clienttalk(void *context);

ChatServer *ChatServer_init(ChatServer *this)
{
    if (this == NULL) {
        NEW(this, ChatServer);
    }

    NEW(this->priv, ChatServer_private);
    this->priv->serverfd = -1;
    this->priv->clientfd_list = ArrayList_init(NULL, -1);
    return this;
}

void ChatServer_free(ChatServer *this, BOOLEAN dynamic)
{
    int count = ArrayList_getcount(this->priv->clientfd_list);
    int i;
    for (i = count - 1; i >= 0; --i) {
        int *item = (int *)ArrayList_remove(this->priv->clientfd_list, i);
        free(item);
    }
    
    ArrayList_free(this->priv->clientfd_list, TRUE);
    free(this->priv);

    if (dynamic == TRUE) {
        free(this);
    }
}

void ChatServer_load(ChatServer *this)
{
    this->priv->serverfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->priv->serverfd == -1) {
        perror("Cannot create server socket");
        exit(EXIT_FAILURE);
    }

    int allowreuse = 1;
    if (setsockopt(this->priv->serverfd, SOL_SOCKET, SO_REUSEADDR, &allowreuse,
                   sizeof(allowreuse)) == -1) {
        perror("Cannot manipulate server socket");
        close(this->priv->serverfd);
        exit(EXIT_FAILURE);
    }
}

void ChatServer_start(ChatServer *this)
{
    struct sockaddr_in myaddr;
    
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(DEFAULT_PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->priv->serverfd, (const struct sockaddr *)&myaddr,
             sizeof(struct sockaddr_in)) == -1) {
        perror("Cannot bind address to server socket");
        close(this->priv->serverfd);
        exit(EXIT_FAILURE);
    }

    if (listen(this->priv->serverfd, 10) == -1) {
        perror("Cannot listen for connection using server socket");
        close(this->priv->serverfd);
        exit(EXIT_FAILURE);
    }

    printf("Awaiting for client connections...\n");
    ChatServer_acceptclients(this);
}

void ChatServer_stop(ChatServer *this)
{
    shutdown(this->priv->serverfd, SHUT_RDWR);
    close(this->priv->serverfd);
}

void ChatServer_acceptclients(ChatServer *this)
{
    for (;;) {
        int *clientfd = (int *)malloc(sizeof(int));
        *clientfd = accept(this->priv->serverfd, NULL, NULL);
        if (*clientfd < 0) {
            perror("Error accepting client connection");
            close(this->priv->serverfd);
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        ArrayList_add(this->priv->clientfd_list, clientfd);
        NEW_I(context, ThreadContext);
        context->instance = this;
        context->clientindex = ArrayList_getcount(
                this->priv->clientfd_list) - 1;

        pthread_t thread;
        pthread_create(&thread, NULL, ChatServer_clienttalk, context);
    }
}

void *ChatServer_clienttalk(void* context)
{
    ThreadContext *tcontext = (ThreadContext *)context;
    int clientfd = *(int *)ArrayList_get(
        tcontext->instance->priv->clientfd_list, tcontext->clientindex);

    while (TRUE) {
        char mymsg[] = "Command: ";
        if (write(clientfd, mymsg, strlen(mymsg)) < 0) {
            perror("Cannot send message to client");
            break;
        }

        char climsg[256];
        bzero(climsg, 256);
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
