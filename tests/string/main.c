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
void show_string(const char *name, string_t *str);

int main(void)
{
    char str1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char str2[] = "0123456789";
    char str3[] = "utf-8 áéíóúü ÁÉÍÓÚÜ €𤭢"; // 22 chars
    char str4[] = "€ÁÉÍÓÚÜ";
    char str5[] = "Á€ÉÍÓÚÜ";
    char str6[] = "Á𤭢ÉÍÓÚÜ";
    char str7[] = "/home/user;/usr/local:/usr/share/icons;;/sbin/";

    string_t *tmpstr1, *tmpstr2;
    char *tmps1;
    string_ll_t *strll1, *strll2, *strll3, *strll4;

    // =========================================================================
    tmpstr1 = string_create(str1);
    show_string("1 - create", tmpstr1);     // 52 chars

    // =========================================================================
    tmpstr2 = string_concat(tmpstr1, string_create(str2));  // <-- mem leak
    show_string("2 - concat", tmpstr2);     // 62 chars

    string_free(tmpstr1);

    // =========================================================================
    tmpstr1 = string_substring(tmpstr2, 26, 26);
    show_string("3 - substring", tmpstr1);      // 26 chars

    string_free(tmpstr2);
    string_free(tmpstr1);

    // =========================================================================
    tmpstr1 = string_create(str3);
    show_string("4 - create (utf-8)", tmpstr1);     // 22 chars

    // =========================================================================
    tmpstr2 = string_substring(tmpstr1, 6, 3);
    show_string("5 - substring (utf-8)", tmpstr2);  // 3 chars

    string_free(tmpstr2);
    string_free(tmpstr1);

    // =========================================================================
    tmpstr1 = string_create(str4);
    tmpstr2 = string_substring(tmpstr1, 0, 3);
    show_string("6 - substring (utf-8)", tmpstr2);  // 3 chars

    string_free(tmpstr2);
    string_free(tmpstr1);

    // =========================================================================
    tmpstr1 = string_create(str5);
    tmpstr2 = string_substring(tmpstr1, 0, 3);
    show_string("7 - substring (utf-8)", tmpstr2);  // 3 chars

    string_free(tmpstr2);
    string_free(tmpstr1);

    // =========================================================================
    tmpstr1 = string_create(str6);
    tmpstr2 = string_substring(tmpstr1, 0, 4);
    show_string("8 - substring (utf-8)", tmpstr2);  // 4 chars

    string_free(tmpstr2);
    string_free(tmpstr1);

    // =========================================================================
    tmps1 = pchar_substring(str1, 5, 3);  // FGH
    tmpstr1 = string_create(tmps1);
    show_string("9 - substring (pchar)", tmpstr1);  // 3 chars

    string_free(tmpstr1);
    free(tmps1);

    // =========================================================================
    tmpstr1 = string_create(str3);
    strll1 = string_split(tmpstr1, " ");
    strll2 = strll1;

    while (strll2) {
        show_string("10 - split (utf-8)", strll2->node);
        strll2 = strll2->next;
    }

    string_free(tmpstr1);
    string_ll_free(strll1);

    // =========================================================================
    tmpstr1 = string_create(str7);
    strll1 = string_split(tmpstr1, ":;");
    strll3 = strll1;

    printf("Test: 11 - split\n");
    printf("Text: %s\n", string_get(tmpstr1));

    while (strll3) {
        printf("\t%s\n", string_get(strll3->node));

        strll2 = string_split(strll3->node, "/");
        strll4 = strll2;

        while (strll4) {
            printf("\t\t%s\n", string_get(strll4->node));
            strll4 = strll4->next;
        }

        string_ll_free(strll2);
        strll3 = strll3->next;
    }

    string_free(tmpstr1);
    string_ll_free(strll1);
    printf("\n");

    return 0;
}

void show_string(const char *name, string_t *str)
{
    printf("Test: %s\n", name);
    printf("Length: %d\n", string_length(str));
    printf("char length: %d\n", strlen(string_get(str)));
    printf("Value: %s\n\n", string_get(str));
}

