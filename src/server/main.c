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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

#ifdef WIN32
#include "windef.h"
#include <string.h>
#endif

#include "chatserver.h"

chatserver_t *server;
void on_signal(int);

int main(void)
{
    #ifdef WIN32
    WSADATA wsadata;
    memset(&wsadata, 0, sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }
    #endif

    // Catch CTRL+C
    signal(SIGINT, on_signal);

    server = chatserver_create();
    chatserver_load(server);
    chatserver_start(server);
    chatserver_free(server);

    #ifdef WIN32
    WSACleanup();
    #endif

    return EXIT_SUCCESS;
}

void on_signal(int signal)
{
    printf("\n\nSignal caught: %d.\n", signal);
    
    if (signal == SIGINT) {
        chatserver_stop(server);
    }
}

