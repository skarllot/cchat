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

#ifndef _STRING_H
#define _STRING_H

#include "basic.h"

typedef struct _String String;
typedef struct _String_private String_private;

struct _String
{
    String_private *priv;
};

String *String_init(String *this, const char *str);
void String_free(String *this, BOOLEAN dynamic);

String *String_concat(String *str1, String *str2);
const char *String_get(String *this);
int String_length(String *this);
String *String_substring(String *this, int index, int length);

#endif /* _STRING_H */

