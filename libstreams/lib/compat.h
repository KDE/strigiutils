/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _STRIGI_COMPAT_H
#define _STRIGI_COMPAT_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <strigi/strigiconfig.h>

#ifndef HAVE_ISBLANK
int isblank(char c);
#endif

#ifndef HAVE_SETENV
int setenv(const char *name, const char *value, int overwrite);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, int l);
#endif

#ifndef HAVE_STRCASESTR
const char *strcasestr(const char *big, const char *little);
#endif

#ifndef HAVE_MKSTEMP
int mkstemp(char *tmpl);
#endif

#endif // _STRIGI_COMPAT_H
