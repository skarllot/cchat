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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 1100

struct _ChatServer_private
{
    int serverfd;
};

ChatServer *ChatServer_init(ChatServer *this)
{
    if (this == NULL) {
        this = (ChatServer *)malloc(sizeof(ChatServer));
    }

    this->priv = (ChatServer_private *)malloc(sizeof(ChatServer_private));
    this->priv->serverfd = -1;
    return this;
}

void ChatServer_free(ChatServer *this, BOOLEAN dynamic)
{
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
    for (;;) {
        int clientfd = accept(this->priv->serverfd, NULL, NULL);
        if (clientfd < 0) {
            perror("Error accepting client connection");
            close(this->priv->serverfd);
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        char mymsg[] = "I'm the server!\n";
        if (write(clientfd, mymsg, strlen(mymsg)) < 0) {
            perror("Cannot send message to client");
        }
        char climsg[256];
        bzero(climsg, 256);
        if (read(clientfd, climsg, 255) < 0) {
            perror("Cannot receive message from client");
        }
        printf("Client message: %s\n", climsg);

        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
        printf("Client disconnected.\n");
    }
}

void ChatServer_stop(ChatServer *this)
{
    shutdown(this->priv->serverfd, SHUT_RDWR);
    close(this->priv->serverfd);
}
