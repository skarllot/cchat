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

    String *tmpstr1, *tmpstr2;

    // =========================================================================
    tmpstr1 = String_init(NULL, str1);
    show_string(tmpstr1);

    // =========================================================================
    tmpstr2 = String_concat(tmpstr1, String_init(NULL, str2));  // <-- mem leak
    show_string(tmpstr2);

    String_free(tmpstr1, TRUE);

    // =========================================================================
    tmpstr1 = String_substring(tmpstr2, 26, 26);
    show_string(tmpstr1);

    String_free(tmpstr2, TRUE);
    String_free(tmpstr1, TRUE);

    // =========================================================================
    tmpstr1 = String_init(NULL, str3);
    show_string(tmpstr1);

    // =========================================================================
    tmpstr2 = String_substring(tmpstr1, 6, 3);
    show_string(tmpstr2);

    String_free(tmpstr2, TRUE);
    String_free(tmpstr1, TRUE);

    // =========================================================================
    tmpstr1 = String_init(NULL, str4);
    tmpstr2 = String_substring(tmpstr1, 0, 3);
    show_string(tmpstr2);

    String_free(tmpstr2, TRUE);
    String_free(tmpstr1, TRUE);

    // =========================================================================
    tmpstr1 = String_init(NULL, str5);
    tmpstr2 = String_substring(tmpstr1, 0, 3);
    show_string(tmpstr2);

    String_free(tmpstr2, TRUE);
    String_free(tmpstr1, TRUE);

    // =========================================================================
    tmpstr1 = String_init(NULL, str6);
    tmpstr2 = String_substring(tmpstr1, 0, 4);
    show_string(tmpstr2);

    String_free(tmpstr2, TRUE);
    String_free(tmpstr1, TRUE);

    return 0;
}

void show_string(String *str)
{
    printf("=========================\n");
    printf("Length: %d\n", String_length(str));
    printf("Value: %s\n", String_get(str));
    printf("=========================\n");
}

