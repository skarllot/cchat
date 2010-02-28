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

#include <stdio.h>
#include <stdlib.h>

#include "../../src/common/arraylist.h"

void show_array(ArrayList *array);

int main(void)
{
    int a = 15, b = 22, c = 84, d = 23, e = 9, f = 212;

    ArrayList *array = ArrayList_init(NULL);
    ArrayList_add(array, &a);
    ArrayList_add(array, &b);
    ArrayList_add(array, &c);
    ArrayList_add(array, &d);
    ArrayList_add(array, &e);
    ArrayList_add(array, &f);

    show_array(array);
    ArrayList_remove(array, 0);
    show_array(array);
    ArrayList_remove(array, 2);
    show_array(array);
    
    ArrayList_free(array, TRUE);
}

void show_array(ArrayList* array)
{
    printf("=========================\n");
    int i;
    for (i = 0; i < ArrayList_getcount(array); ++i) {
        printf("array[%i] = %i\n", i, *(int *)ArrayList_get(array, i));
    }
    printf("=========================\n");
}
