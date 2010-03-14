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

// Count characters from UTF-8 string.
static int strlen_utf8(const char *s);
// Count bytes by given length from UTF-8 string.
static int strlen_utf8_bytes(const char *s, int len);

struct _String_private
{
    char *string;
    int length;
    int capacity;
    // CHANGE: concat(), substring().
};

String *String_init(String *this, const char *str)
{
    if (this == NULL) {
        MALLOC(this, String);
    }

    MALLOC(this->priv, String_private);
    if (str != NULL) {
        int lenbytes = strlen(str) + 1;

        this->priv->string = (char *)malloc(lenbytes);
        memset(this->priv->string, 0, lenbytes);
        strcpy(this->priv->string, str);

        this->priv->length = strlen_utf8(str);
        this->priv->capacity = lenbytes;
    }
    else {
        this->priv->length = -1;
        this->priv->capacity = 0;
    }
    return this;
}

void String_free(String *this, BOOLEAN dynamic)
{
    free(this->priv->string);
    free(this->priv);
    
    if (dynamic == TRUE) {
        free(this);
    }
}

String *String_concat(String *str1, String *str2)
{
    int tsize = strlen(str1->priv->string) + strlen(str2->priv->string) + 1;
    char *newstr =  (char *)malloc(tsize);
    memset(newstr, 0, tsize);
    strcpy(newstr, str1->priv->string);
    strcat(newstr, str2->priv->string);

    String *ret = String_init(NULL, NULL);
    ret->priv->string = newstr;
    ret->priv->length = strlen_utf8(newstr);
    ret->priv->capacity = tsize;
    return ret;
}

const char *String_get(String *this)
{
    return this->priv->string;
}

int String_length(String *this)
{
    return this->priv->length;
}

String *String_substring(String *this, int index, int length)
{
    if (index + length > this->priv->length) {
        // TODO: Needs a exception handler.
        fprintf(stderr, "Out of range substring\n");
        exit(EXIT_FAILURE);
    }

    int realidx = strlen_utf8_bytes(this->priv->string, index);
    int reallen = strlen_utf8_bytes(&this->priv->string[realidx], length);

    char *newstr = (char *)malloc(reallen + 1);
    memset(newstr, 0, reallen + 1);
    memcpy(newstr, &this->priv->string[realidx], reallen);

    String *ret = String_init(NULL, NULL);
    ret->priv->string = newstr;
    ret->priv->length = strlen_utf8(newstr);
    ret->priv->capacity = reallen + 1;
    return ret;
}

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

