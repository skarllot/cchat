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

#ifndef _BASIC_H
#define _BASIC_H

#include <stdlib.h>
#include <string.h>

#define NEW(p, type) \
    p = (type *)malloc(sizeof(type)); \
    memset(p, 0, sizeof(type));
#define NEW_I(p, type) \
    type *p; \
    NEW(p, type);

#define TRUE 1
#define FALSE 0
typedef int BOOLEAN;

#endif /* _BASIC_H */

