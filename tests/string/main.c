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

#include <stdio.h>

#include "common/cstring.h"

// Shows up string.
void show_string(String *str);

int main(void)
{
    char str1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char str2[] = "0123456789";
    char str3[] = "utf-8 áéíóúü ÁÉÍÓÚÜ €𤭢"; // 22 chars
    char str4[] = "€ÁÉÍÓÚÜ";
    char str5[] = "Á€ÉÍÓÚÜ";
    char str6[] = "Á𤭢ÉÍÓÚÜ";
    String *mystr = String_init(NULL, str1);
    show_string(mystr);

    String *mystr2 = String_concat(mystr, String_init(NULL, str2));
    show_string(mystr2);

    String *mystr3 = String_substring(mystr2, 26, 26);
    show_string(mystr3);

    String *mystr4 = String_init(NULL, str3);
    show_string(mystr4);

    String *mystr5 = String_substring(mystr4, 6, 3);
    show_string(mystr5);

    String *mystr6 = String_init(NULL, str4);
    show_string(String_substring(mystr6, 0, 3));

    String *mystr7 = String_init(NULL, str5);
    show_string(String_substring(mystr7, 0, 3));

    String *mystr8 = String_init(NULL, str6);
    show_string(String_substring(mystr8, 0, 4));

    return 0;
}

void show_string(String *str)
{
    printf("=========================\n");
    printf("Length: %d\n", String_length(str));
    printf("Value: %s\n", String_get(str));
    printf("=========================\n");
}

