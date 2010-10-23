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

#ifdef WIN32

#ifndef _WINDEF_H
#define _WINDEF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WINVER
// Windows Server 2003 with SP1, Windows XP with SP2
// See http://msdn.microsoft.com/en-us/library/aa383745.aspx
#define WINVER 0x0502
#endif /* WINVER */

#if WINVER < 0x0502
#error WINVER should be greater or equal 0x0502.
#endif

#include <windows.h>
#include <winsock2.h>

#define malloc(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define free(x) HeapFree(GetProcessHeap(), 0, (x))

#endif /* _WINDEF_H */

#endif /* WIN32 */

