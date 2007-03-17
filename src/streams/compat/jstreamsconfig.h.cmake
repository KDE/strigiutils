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

// either use stdint.h if available or define by our own
#if @STRIGI_HAVE_STDINT_H@
 #include <stdint.h>
#endif

// our needed types
#if !@HAVE_INT16_T@
 #define HAVE_INT16_T 1
 #if ${SIZEOF_SHORT}==2 //is short 2bits?
  typedef short int16_t;
 #else
  #error Could not determine type for int16_t!
 #endif
#endif

#if !@HAVE_UINT16_T@
 #define HAVE_UINT16_T 1
 #if ${SIZEOF_SHORT}==2 //is short 2bits?
  typedef unsigned short uint16_t;
 #else
  #error Could not determine type for uint16_t!
 #endif
#endif

#if !@HAVE_INT32_T@
 #define HAVE_INT32_T 1
 #if ${SIZEOF_INT}==4 //is int 4bits?
  typedef int int32_t;
 #elif ${SIZEOF_LONG}==4 //is long 4bits?
  typedef long int32_t;
 #else
  #error Could not determine type for int32_t!
 #endif
#endif

#if !@HAVE_UINT32_T@
 #define HAVE_UINT32_T 1
 #if ${SIZEOF_INT}==4 //is int 4bits?
  typedef unsigned int uint32_t;
 #elif ${SIZEOF_LONG}==4 //is long 4bits?
  typedef unsigned long uint32_t;
 #else
  #error Could not determine type for uint32_t!
 #endif
#endif

#if !@HAVE_INT64_T@
 #define HAVE_INT64_T 1
 #if ${SIZEOF_LONG}==8
  typedef long int64_t;
 #elif ${SIZEOF_LONGLONG}==8
  typedef long long int64_t; 
 #else
  #error Could not determine type for int64_t!
 #endif
#endif

#if !@HAVE_UINT64_T@
 #define HAVE_UINT64_T 1
 #if ${SIZEOF_LONG}==8
  typedef unsigned long uint64_t;
 #elif ${SIZEOF_LONGLONG}==8
  typedef unsigned long long uint64_t; 
 #elif defined(HAVE___INT64)
  typedef unsigned __int64 uint64_t; 
 #else
  #error Could not determine type for uint64_t!
 #endif
#endif

#if !@HAVE_UINT@
 typedef unsigned int uint;
 #define HAVE_UINT 1
#endif

#if !@HAVE_INTPTR_T@
 typedef int intptr_t;
 #define HAVE_INTPTR_T 1
#endif

#if !@HAVE_SOCKLEN_T@
 typedef int socklen_t;
 #define HAVE_SOCKLEN_T 1
#endif

#endif //JSTREAMSCONFIG_H
