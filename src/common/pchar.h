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

#ifndef _PCHAR_H
#define _PCHAR_H

typedef struct _pchar_ll_t pchar_ll_t;

struct _pchar_ll_t
{
    char *node;
    pchar_ll_t *next;
};

void pchar_ll_free(pchar_ll_t *sll);

/** Gets a copy of specified string.
 */
char *pchar_copy(const char *s);
/** Gets the index of first occurrence of specified character.
 *
 * @param c A character to seek.
 */
int pchar_index_of(const char *s, char c);
/** Remove line breaks from string.
 *
 * @param s The string.
 */
void pchar_remove_lbreaks(char *s);
/** Split string into linked list tokens.
 *
 * @param delimiters Delimiters array.
 */
pchar_ll_t *pchar_split(const char *s, const char *delimiters);
/** Gets a substring from string.
 *
 * @param s The string.
 * @param index String index where substring begins.
 * @param length Substring length.
 */
char *pchar_substring(const char *s, int index, int length);

/** Append a string to linked list.
 *
 * @param str String to append.
 */
void pchar_ll_append(pchar_ll_t *sll, const char *s);

#endif /* _PCHAR_H */

