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

#ifndef _CSTRING_H
#define _CSTRING_H

#include "basic.h"

typedef struct _string_t string_t;
typedef struct _string_it string_it;
typedef struct _string_ll_t string_ll_t;

struct _string_t
{
    string_it *priv;
};

struct _string_ll_t
{
    string_t *node;
    string_ll_t *next;
};

string_t *string_create(const char *s);
string_t *string_create_c(int capacity);
void string_free(string_t *str);
void string_ll_free(string_ll_t *strll);

/** Remove line breaks from string.
 *
 * @param s The string.
 */
void pchar_remove_lbreaks(char *s);
/** Gets a substring from string.
 *
 * @param s The string.
 * @param index String index where substring begins.
 * @param length Substring length.
 */
char *pchar_substring(const char *s, int index, int length);

/** Concatenate two strings.
 *
 * @param str1 First string.
 * @param str2 Second string.
 */
string_t *string_concat(string_t *str1, string_t *str2);
/** Gets char pointer from string.
 *
 */
const char *string_get(string_t *str);
/** Gets string length.
 *
 */
int string_length(string_t *str);
/** Split string into linked list tokens.
 *
 * @param delimiters Delimiters array.
 */
string_ll_t *string_split(string_t *str, const char *delimiters);
/** Gets a substring from string.
 *
 * @param index String index where substring begins.
 * @param length Substring length.
 */
string_t *string_substring(string_t *str, int index, int length);

#endif /* _CSTRING_H */

