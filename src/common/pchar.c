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

#include "pchar.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "basic.h"

void pchar_ll_free(pchar_ll_t *sll)
{
    pchar_ll_t *tmp;

    while (sll) {
        if (sll->node)
            free(sll->node);

        tmp = sll;
        sll = sll->next;
        free(tmp);
    }
}

char *pchar_copy(const char *s)
{
    int slen = strlen(s);

    char *newstr = (char *)malloc(slen + 1);
    memset(newstr, 0, slen + 1);
    memcpy(newstr, s, slen);

    return newstr;
}

int pchar_index_of(const char *s, char c)
{
    register int i = 0;
    while (s[i]) {
        if (s[i] == c)
            return i;

        i++;
    }

    return -1;
}

void pchar_remove_lbreaks(char *s)
{
    register int i = 0;
    while (s[i]) {
        if (s[i] == '\n' ||
            s[i] == '\r')
            s[i] = '\0';

        i++;
    }
}

pchar_ll_t *pchar_split(const char *s, const char *delimiters)
{
    pchar_ll_t *ret = NULL;
    pchar_ll_t *curr = NULL;

    int i = 0, ini = 0, j = 0, len = -1;

    while (1) {
        j = 0;
        while (delimiters[j]) {
            if (s[i] == delimiters[j] || s[i] == 0) {
                if (ini == i) {
                    ini++;
                    break;
                }

                if (curr == NULL) {
                    MALLOC(curr, pchar_ll_t);
                    ret = curr;
                }
                else {
                    MALLOC(curr->next, pchar_ll_t);
                    curr = curr->next;
                }

                len = i - ini;
                curr->node = pchar_substring(s, ini, len);
                curr->next = NULL;

                ini = i + 1;
                break;
            }
            j++;
        }
        if (!s[i])
            break;
        i++;
    }

    return ret;
}

char *pchar_substring(const char *s, int index, int length)
{
    int slen = strlen(s);
    if (index + length > slen) {
        // TODO: Needs a exception handler.
        fprintf(stderr, "Out of range substring\n");
        return NULL;
    }

    char *newstr = (char *)malloc(length + 1);
    memset(newstr, 0, length + 1);
    memcpy(newstr, &s[index], length);

    return newstr;
}

void pchar_tolower(char *s)
{
    register int i = 0;
    while (s[i]) {
        s[i] = tolower(s[i]);
        i++;
    }
}

void pchar_trim_spaces(char *s)
{
    register int i = 0;
    int len = strlen(s);
    int idx_f = -1, idx_l = -1;
    
    while (s[i]) {
        if (!isspace(s[i])) {
            idx_f = i;
            break;
        }
        i++;
    }
    
    // Empty string
    if (idx_f == -1) {
        s[0] = '\0';
        return;
    }
    
    i = len-1;
    while (i != -1) {
        if (!isspace(s[i])) {
            idx_l = i;
            break;
        }
        i--;
    }
    
    if (idx_l - idx_f + 1 == len)
        return;
    
    memmove(s, &s[idx_f], idx_l - idx_f + 1);
    if (idx_l + 1 < len)
        s[idx_l - idx_f + 1] = '\0';
}

void pchar_ll_append(pchar_ll_t *sll, const char *s)
{
    pchar_ll_t *curr = sll;
    while (curr->next)
        curr = curr->next;

    MALLOC(curr->next, pchar_ll_t);
    curr = curr->next;
    curr->node = pchar_copy(s);
    curr->next = NULL;
}

