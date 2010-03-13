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
#define _ARRAYLIST_H

#include "basic.h"

typedef struct _ArrayList ArrayList;
typedef struct _ArrayList_private ArrayList_private;

struct _ArrayList
{
    ArrayList_private *priv;
};

ArrayList *ArrayList_init(ArrayList *this, int size);
void ArrayList_free(ArrayList *this, BOOLEAN dynamic);

/** Adds a new item to array list.
 *
 * @param item The item to add.
 */
void ArrayList_add(ArrayList *this, const void *item);
/** Remove all items from array list.
 */
void ArrayList_clean(ArrayList *this);
/** Gets a item from array list.
 *
 * @param index Item index to get.
 * @return The requested item.
 */
const void *ArrayList_get(ArrayList *this, int index);
/** Gets allocated space to array list.
 *
 * @return Allocated space into items count.
 */
int ArrayList_getcapacity(ArrayList *this);
/** Gets allocated items into array list.
 *
 * @return Count of allocated items.
 */
int ArrayList_getcount(ArrayList *this);
/** Inserts a new item into array list, without overwrite.
 *
 * @param index Index to insert the new item.
 * @param item The item to insert.
 */
void ArrayList_insert(ArrayList *this, int index, const void *item);
/** Removes a item from array list.
 *
 * @param index Item index to remove.
 * @return Removed item.
 */
const void *ArrayList_remove(ArrayList *this, int index);
/** Sets a new item at specified index.
 *
 * @param index Item index to overwrite.
 * @param item The new item.
 */
void ArrayList_set(ArrayList *this, int index, const void *item);
/** Changes array list allocated space to match items count.
 */
void ArrayList_trim(ArrayList *this);

#endif /* _ARRAYLIST_H */

