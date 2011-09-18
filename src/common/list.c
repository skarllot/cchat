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

#include <stdio.h>

#include "list.h"

struct _list_it
{
    const void **data;
    int num_elements;
    int num_allocated;
};

// Allocs more space if requested size is greater than allocated
static void list_alloc(list_t *lst, int size);
// Get index of specified item from array list.
static int list_getindex(list_t *lst, const void *item);
// Check if requested index is a valid index into specified array list.
static void list_validateindex(list_t *lst, int index);

list_t *list_create(int size)
{
    list_t *lst;
    MALLOC(lst, list_t);

    MALLOC(lst->priv, list_it);
    lst->priv->data = NULL;
    lst->priv->num_elements = 0;
    lst->priv->num_allocated = 0;
    list_alloc(lst, size);

    return lst;
}

void list_free(list_t *lst)
{
    free(lst->priv->data);
    free(lst->priv);

    free(lst);
}

void list_add(list_t *lst, const void *item)
{
    list_alloc(lst, lst->priv->num_elements + 1);

    lst->priv->data[lst->priv->num_elements] = item;
    lst->priv->num_elements++;
}

void list_clean(list_t *lst)
{
    int i;
    for (i = 0; i < lst->priv->num_elements; ++i) {
        lst->priv->data[i] = NULL;
    }

    lst->priv->num_elements = 0;
}

const void *list_get(list_t *lst, int index)
{
    list_validateindex(lst, index);
    return lst->priv->data[index];
}

int list_getindex(list_t *lst, const void *item)
{
    int i, count = lst->priv->num_elements;
    for (i = 0; i < count; i++) {
        if (lst->priv->data[i] == item)
            return i;
    }

    return -1;
}

int list_getcapacity(list_t *lst)
{
    return lst->priv->num_allocated;
}

int list_getcount(list_t *lst)
{
    return lst->priv->num_elements;
}

void list_insert(list_t *lst, int index, const void *item)
{
    list_validateindex(lst, index);
    // ensure space to the new item
    list_alloc(lst, lst->priv->num_elements + 1);

    int i;
    // Shifts items to the right
    for (i = lst->priv->num_elements; i > index; --i) {
        lst->priv->data[i] = lst->priv->data[i - 1];
    }
    lst->priv->data[index] = item;
    lst->priv->num_elements++;
}

void list_remove(list_t *lst, const void *item)
{
    int index = list_getindex(lst, item);
    if (index == -1) {
        perror("Attempt to remove a not found item from list_t");
        exit(EXIT_FAILURE);
    }

    list_remove_at(lst, index);
}

const void *list_remove_at(list_t *lst, int index)
{
    list_validateindex(lst, index);

    const void *ret = lst->priv->data[index];
    lst->priv->num_elements--;
    int i;
    // Shifts items to the left
    for (i = index; i < lst->priv->num_elements; ++i) {
        lst->priv->data[i] = lst->priv->data[i + 1];
    }
    lst->priv->data[lst->priv->num_elements] = NULL;

    return ret;
}

void list_set(list_t *lst, int index, const void *item)
{
    list_validateindex(lst, index);

    lst->priv->data[index] = item;
}

void list_trim(list_t *lst)
{
    void *_tmp = realloc(lst->priv->data,
                         lst->priv->num_elements * sizeof(void *));
    if (!_tmp) {
        perror("Couldn't alloc memory");
        exit(EXIT_FAILURE);
    }

    lst->priv->data = (const void **)_tmp;
    lst->priv->num_allocated = lst->priv->num_elements;
}

void list_alloc(list_t *lst, int size)
{
    // Check whether already have enough space
    if (size <= lst->priv->num_allocated) {
        return;
    }

    // Allocate space to 4 items iniatially
    if (lst->priv->num_allocated == 0) {
        lst->priv->num_allocated = 4;
    }
    // Double allocated space while have not enough space
    while (size > lst->priv->num_allocated) {
        lst->priv->num_allocated *= 2;
    }

    void *_tmp = realloc(lst->priv->data,
                         lst->priv->num_allocated * sizeof(void *));
    if (!_tmp) {
        perror("Couldn't alloc memory");
        exit(EXIT_FAILURE);
    }

    lst->priv->data = (const void **)_tmp;
}

void list_validateindex(list_t *lst, int index)
{
    // Exits program if a invalid index is specified.
    if (index < 0 || index >= lst->priv->num_elements) {
        // TODO: Needs a exception handler.
        fprintf(stderr, "Out of range array index\n");
        exit(EXIT_FAILURE);
    }
}

