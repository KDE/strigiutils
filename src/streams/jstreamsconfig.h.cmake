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
 
 #ifndef JSTREAMSCONFIG_H
 #define JSTREAMSCONFIG_H

//autoconfigure stuff:
#cmakedefine HAVE_STRNCASECMP
#cmakedefine HAVE_STRCASESTR
#cmakedefine HAVE_ISBLANK
#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_SYS_NDIR_H
#cmakedefine HAVE_SYS_DIR_H
#cmakedefine HAVE_NDIR_H
#cmakedefine HAVE_DIRENT_H
#cmakedefine HAVE_WINDOWS_H
#cmakedefine HAVE_DLFCN_H

#cmakedefine CMAKE_ANSI_FOR_SCOPE

#cmakedefine HAVE_INT64_T
#cmakedefine HAVE_UINT64_T
#cmakedefine HAVE_INT32_T
#cmakedefine HAVE_UINT32_T
#cmakedefine HAVE_UINT
#cmakedefine HAVE_SOCKLEN_T
#cmakedefine HAVE___INT64
#cmakedefine HAVE_INTPTR_T

#ifndef HAVE_INT64_T
 #if defined(HAVE___INT64)
  typedef __int64 int64_t; 
  typedef unsigned __int64 uint64_t;
 #elif 0${SIZEOF_LONGLONG}==8
  typedef long long int64_t; 
  typedef unsigned long long uint64_t;
 #endif
#endif

#ifndef HAVE_INT32_t
 #if 0${SIZEOF_LONG}==4
  typedef long int32_t;
  typedef unsigned long uint32_t;
 #elif 0${SIZEOF_INT}==4
  typedef int int32_t;
  typedef unsigned int uint32_t;
 #endif
#endif

#ifndef HAVE_UINT
 typedef unsigned int uint;
#endif

#ifndef HAVE_INTPTR_T
 typedef long intptr_t;
#endif

#ifndef HAVE_SOCKLEN_T
typedef int socklen_t;
#endif

#ifndef HAVE_STRCASESTR
int strncasecmp(const char* a, const char* b, int l);
#endif

#ifndef HAVE_STRCASESTR
const char * strcasestr(const char *big, const char *little);
#endif

#ifndef HAVE_ISBLANK
bool isblank(char c);
#endif

#ifndef CMAKE_ANSI_FOR_SCOPE
 #define for if (0); else for
#endif

#if defined(HAVE_WINDOWS_H) && !defined(__CYGWIN__)
 #include <windows.h>
#endif

#endif