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

#include "cstring.h"
#include <stdio.h>
#include <string.h>

// Create string and reuse char array (non copying)
static string_t *string_create_nc(const char *s);
// Count characters from UTF-8 string.
static int strlen_utf8(const char *s);
// Count bytes by given length from UTF-8 string.
static int strlen_utf8_bytes(const char *s, int len);

struct _string_it
{
    char *string;
    int length;
    int capacity;
};

string_t *string_create(const char *s)
{
    char *s2 = NULL;
    if (s != NULL) {
        int lenbytes = (strlen(s) + 1) * sizeof(char);

        s2 = (char *)malloc(lenbytes);
        memset(s2, 0, lenbytes);
        strcpy(s2, s);
    }

    return string_create_nc(s2);
}

string_t *string_create_nc(const char *s)
{
    string_t *str;
    MALLOC(str, string_t);

    MALLOC(str->priv, string_it);
    if (s != NULL) {
        str->priv->string = s;
        str->priv->length = strlen_utf8(s);
        str->priv->capacity = strlen(s) + 1;
    }
    else {
        str->priv->length = -1;
        str->priv->capacity = 0;
    }
    return str;
}

string_t *string_create_c(int capacity)
{
    char *s = (char *)malloc(sizeof(char) * capacity);
    memset(s, 0, sizeof(char) * capacity);

    return string_create_nc(s);
}

void string_free(string_t *str)
{
    free(str->priv->string);
    free(str->priv);

    free(str);
}

void string_ll_free(string_ll_t *strll)
{
    string_ll_t *tmp;

    while (strll) {
        if (strll->node)
            string_free(strll->node);

        tmp = strll;
        strll = strll->next;
        free(tmp);
    }
}

string_t *string_concat(string_t *str1, string_t *str2)
{
    int tsize = strlen(str1->priv->string) + strlen(str2->priv->string) + 1;
    char *newstr =  (char *)malloc(tsize);
    memset(newstr, 0, tsize);
    strcpy(newstr, str1->priv->string);
    strcat(newstr, str2->priv->string);

    return string_create_nc(newstr);
}

const char *string_get(string_t *str)
{
    return str->priv->string;
}

int string_length(string_t *str)
{
    return str->priv->length;
}

string_ll_t *string_split(string_t *str, const char *delimiters)
{
    string_ll_t *ret = NULL;
    string_ll_t *curr = NULL;

    char *s = str->priv->string;
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
                    MALLOC(curr, string_ll_t);
                    ret = curr;
                }
                else {
                    MALLOC(curr->next, string_ll_t);
                    curr = curr->next;
                }

                len = i - ini;
                curr->node = string_create_nc(pchar_substring(s, ini, len));
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

string_t *string_substring(string_t *str, int index, int length)
{
    if (index + length > str->priv->length) {
        // TODO: Needs a exception handler.
        fprintf(stderr, "Out of range substring\n");
        exit(EXIT_FAILURE);
    }

    int realidx = strlen_utf8_bytes(str->priv->string, index);
    int reallen = strlen_utf8_bytes(&str->priv->string[realidx], length);

    char *newstr = (char *)malloc(reallen + 1);
    memset(newstr, 0, reallen + 1);
    memcpy(newstr, &str->priv->string[realidx], reallen);

    return string_create_nc(newstr);
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

// Calculate length in UTF-8 characters
// See http://en.wikipedia.org/wiki/UTF-8#Description
int strlen_utf8(const char *s)
{
    int i = 0, j = 0;
    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80) j++;
        i++;
    }
    return j;
}

// Calculate bytes from UTF-8 characters length
// See http://en.wikipedia.org/wiki/UTF-8#Description
int strlen_utf8_bytes(const char *s, int len)
{
    int i = 0;
    while (len != 0) {
        if (!s[i]) return -1;

        if ((s[i] & 0x80) == 0x00 || (s[i] & 0xc0) == 0x80) len--;
        else if ((s[i] & 0xf0) == 0xe0) i++;
        else if ((s[i] & 0xf8) == 0xf0) i += 2;

        i++;
    }
    return i;
}

