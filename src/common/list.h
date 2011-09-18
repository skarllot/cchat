/*
 * Copyright (C) 2010-2011 Fabrício Godoy <skarllot@gmail.com>
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

#ifndef _LIST_H
#define _LIST_H

#include "basic.h"

typedef struct _list_t list_t;
typedef struct _list_it list_it;

struct _list_t
{
    list_it *priv;
};

list_t *list_create(int size);
void list_free(list_t *lst);

/** Adds a new item to array list.
 *
 * @param item The item to add.
 */
void list_add(list_t *lst, const void *item);
/** Remove all items from array list.
 */
void list_clean(list_t *lst);
/** Gets a item from array list.
 *
 * @param index Item index to get.
 * @return The requested item.
 */
const void *list_get(list_t *lst, int index);
/** Gets allocated space to array list.
 *
 * @return Allocated space into items count.
 */
int list_getcapacity(list_t *lst);
/** Gets allocated items into array list.
 *
 * @return Count of allocated items.
 */
int list_getcount(list_t *lst);
/** Inserts a new item into array list, without overwrite.
 *
 * @param index Index to insert the new item.
 * @param item The item to insert.
 */
void list_insert(list_t *lst, int index, const void *item);
/** Removes a item from array list.
 * 
 * @param item The item to remove.
 */
void list_remove(list_t *lst, const void *item);
/** Removes a item from array list at specified index.
 *
 * @param index Item index to remove.
 * @return Removed item.
 */
const void *list_remove_at(list_t *lst, int index);
/** Sets a new item at specified index.
 *
 * @param index Item index to overwrite.
 * @param item The new item.
 */
void list_set(list_t *lst, int index, const void *item);
/** Changes array list allocated space to match items count.
 */
void list_trim(list_t *lst);

#endif /* _LIST_H */

