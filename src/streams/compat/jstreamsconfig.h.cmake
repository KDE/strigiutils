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

#cmakedefine HAVE_STRNCASECMP
#cmakedefine HAVE_STRCASESTR
#cmakedefine HAVE_ISBLANK
#cmakedefine HAVE_FCHDIR
#cmakedefine HAVE_NANOSLEEP
#cmakedefine HAVE_STRLWR
#cmakedefine HAVE_GETTIMEOFDAY

#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_SYS_NDIR_H
#cmakedefine HAVE_SYS_DIR_H
#cmakedefine HAVE_NDIR_H
#cmakedefine HAVE_DIRENT_H
#cmakedefine HAVE_WINDOWS_H
#cmakedefine HAVE_DLFCN_H
#cmakedefine HAVE_DIRECT_H

#cmakedefine CMAKE_ANSI_FOR_SCOPE

//////////////////////////////
//thread stuff
//////////////////////////////
#cmakedefine CMAKE_USE_WIN32_THREADS_INIT
#cmakedefine CMAKE_HAVE_PTHREAD_CREATE

//////////////////////////////
//types
//////////////////////////////
#cmakedefine HAVE_INT64_T
#cmakedefine HAVE_INT32_T
#cmakedefine HAVE_SSIZE_T
#cmakedefine HAVE_UINT
#cmakedefine HAVE_SYS_SOCKET_H
#cmakedefine HAVE_SOCKET_H
#cmakedefine HAVE___INT64
#cmakedefine HAVE_INTPTR_T

#ifndef HAVE_INT64_T
 #if defined(HAVE___INT64)
  typedef __int64 int64_t; 
 #elif ${SIZEOF_LONG}==8
  typedef long int64_t;
 #elif ${SIZEOF_LONGLONG}==8
  typedef long long int64_t; 
 #endif
#endif
#ifndef HAVE_UINT64_T
 #if defined(HAVE___UINT64)
  typedef __uint64 uint64_t; 
 #elif ${SIZEOF_LONG}==8
  typedef unsigned long uint64_t;
 #elif ${SIZEOF_LONGLONG}==8
  typedef unsigned long long uint64_t; 
 #elif defined(HAVE___INT64)
  typedef unsigned __int64 uint64_t; 
 #endif
#endif

#ifndef HAVE_INT32_T
 #if ${SIZEOF_INT}==4 //is int 4bits?
  typedef int int32_t;
 #elif ${SIZEOF_LONG}==4 //is long 4bits?
  typedef long int32_t;
 #endif
#endif
#ifndef HAVE_UINT32_T
 #if ${SIZEOF_INT}==4 //is int 4bits?
  typedef unsigned int uint32_t;
 #elif ${SIZEOF_LONG}==4 //is long 4bits?
  typedef unsigned long uint32_t;
 #endif
#endif

#ifndef HAVE_UINT
 typedef unsigned int uint;
#endif

#ifndef HAVE_INTPTR_T
 typedef int intptr_t;
#endif

#ifndef HAVE_SYS_SOCKET_H
typedef int socklen_t;
#endif

//////////////////////////////
//missing functions
//////////////////////////////
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


//////////////////////////////
//windows stuff
//////////////////////////////
#if defined(HAVE_WINDOWS_H) && !defined(__CYGWIN__)

 //need this for ChangeNotify and TryEnterCriticalSection
 //this wont compile for win98 though, but who cares?, not me :)
 #define _WIN32_WINNT 0x400

 #include <windows.h>
 #include <io.h>
 #ifndef snprintf
 	#define snprintf _snprintf
 #endif
 
//for some reason linux is not picking up HAVE_SSIZE_T properly
//but windows always needs it... hack ack
#ifndef HAVE_SSIZE_T
    typedef size_t ssize_t;
#endif
#endif

#ifndef S_ISREG
    #define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
    #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

// set sleep time
#ifdef HAVE_NANOSLEEP
    #define strigi_nanosleep(nanoseconds) struct timespec sleeptime; sleeptime.tv_sec = 0; sleeptime.tv_nsec = nanoseconds; nanosleep(&sleeptime, 0);
#endif


#ifdef _MSC_VER
	#define sleep(x) Sleep(x*1000)
	
	#ifndef strigi_nanosleep
	    #define strigi_nanosleep(nanoseconds) Sleep(nanoseconds/1000000)
	#endif

	#if (_MSC_VER == 1200)
		#pragma warning(disable: 4503) //decorated name length exceeded
		#pragma warning(disable: 4786) //identifier was truncated to '255' characters in the debug information
	#endif
#endif

#endif //JSTREAMSCONFIG_H
