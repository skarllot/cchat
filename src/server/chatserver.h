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

#ifndef _CHATSERVER_H
#define	_CHATSERVER_H

#include "common/basic.h"

typedef struct _ChatServer ChatServer;
typedef struct _ChatServer_private ChatServer_private;

struct _ChatServer
{
    ChatServer_private *priv;
};

ChatServer *ChatServer_init(ChatServer *this);
void ChatServer_free(ChatServer *this, BOOLEAN dynamic);

void ChatServer_load(ChatServer *this);
void ChatServer_start(ChatServer *this);
void ChatServer_stop(ChatServer *this);

#endif	/* _CHATSERVER_H */

