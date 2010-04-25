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

#ifndef _CHATSERVER_H
#define _CHATSERVER_H

#include "common/basic.h"

typedef struct _chatserver_t chatserver_t;
typedef struct _chatserver_it chatserver_it;

struct _chatserver_t
{
    chatserver_it *priv;
};

chatserver_t *chatserver_create();
void chatserver_free(chatserver_t *csrv);

/** Loads server internals.
 */
void chatserver_load(chatserver_t *csrv);
/** Starts server awaiting for client connections.
 */
void chatserver_start(chatserver_t *csrv);
/** Stops server closing all client connections.
 */
void chatserver_stop(chatserver_t *csrv);

#endif /* _CHATSERVER_H */

