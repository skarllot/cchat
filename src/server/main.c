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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

#include "chatserver.h"

ChatServer *server;
void on_signal(int);

int main(void)
{
    // Catch CTRL+C
    signal(SIGINT, on_signal);

    server = ChatServer_init(NULL);
    ChatServer_load(server);
    ChatServer_start(server);
    ChatServer_free(server, TRUE);
    
    return EXIT_SUCCESS;
}

void on_signal(int signal)
{
    printf("\n\nSignal caught: %d.\n", signal);
    
    if (signal == SIGINT) {
        ChatServer_stop(server);
        ChatServer_free(server, TRUE);
    }
}

