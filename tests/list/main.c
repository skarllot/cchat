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
#include <stdlib.h>

#include "common/list.h"

// Shows up array list items.
void show_array(list_t *array);

int main(void)
{
    int a = 15, b = 22, c = 84, d = 23, e = 9, f = 212;

    // Test array list methods
    list_t *array = list_create(-1);
    list_add(array, &a);
    list_add(array, &b);
    list_add(array, &c);
    list_add(array, &d);
    list_add(array, &e);

    show_array(array);
    list_remove_at(array, 0);
    show_array(array);
    list_remove(array, &d);
    show_array(array);
    list_trim(array);
    show_array(array);
    list_insert(array, 1, &f);
    show_array(array);
    list_set(array, 3, &a);
    show_array(array);

    list_free(array);

    // Test array list string handling
    char *msg1 = "First message";
    list_t *array2 = list_create(-1);
    list_add(array2, msg1);
    printf("array2[%d] = %s\n", 0, (char *)list_get(array2, 0));
    printf("array2[%d][%d] = %c\n", 0, 2,
           ((char *)list_get(array2, 0))[2]);
    list_free(array2);

    return 0;
}

void show_array(list_t *array)
{
    printf("=========================\n");
    printf("Count: %d, Capacity: %d\n", list_getcount(array),
           list_getcapacity(array));
    int i;
    for (i = 0; i < list_getcount(array); ++i) {
        printf("array[%d] = %d\n", i, *(int *)list_get(array, i));
    }
    printf("=========================\n");
}
