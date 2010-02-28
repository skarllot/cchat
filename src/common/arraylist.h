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

#ifndef _ARRAYLIST_H
#define	_ARRAYLIST_H

#include "basic.h"

typedef struct _ArrayList ArrayList;
typedef struct _ArrayList_private ArrayList_private;

struct _ArrayList
{
    ArrayList_private *priv;
};

ArrayList *ArrayList_init(ArrayList *this);
void ArrayList_free(ArrayList *this, BOOLEAN dynamic);

void ArrayList_add(ArrayList *this, void *item);
void ArrayList_clean(ArrayList *this);
int ArrayList_getcount(ArrayList *this);
void *ArrayList_get(ArrayList *this, int index);
void ArrayList_remove(ArrayList *this, int index);

#endif	/* _ARRAYLIST_H */

