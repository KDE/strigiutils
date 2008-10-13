/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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
 
#ifndef STRIGICONFIG_H
#define STRIGICONFIG_H

#define STRIGI_VERSION_STRING "@STRIGI_VERSION_STRING@"

/// @brief The major Strigi version number at compile time
#define STRIGI_VERSION_MAJOR @STRIGI_VERSION_MAJOR@

/// @brief The minor Strigi version number at compile time
#define STRIGI_VERSION_MINOR @STRIGI_VERSION_MINOR@

/// @brief The Strigi release version number at compile time
#define STRIGI_VERSION_RELEASE @STRIGI_VERSION_RELEASE@

/**
 * \brief Create a unique number from the major, minor and release number of a %Strigi version
 *
 * This function can be used for preprocessing.
 */
#define STRIGI_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %Strigi Version as a unique number at compile time
 *
 * This macro calculates the %Strigi version into a number. It is mainly used
 * through STRIGI_IS_VERSION in preprocessing.
 */
#define STRIGI_VERSION \
    STRIGI_MAKE_VERSION(STRIGI_VERSION_MAJOR,STRIGI_VERSION_MINOR,STRIGI_VERSION_RELEASE)

/**
 * \brief Check if the %Strigi version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if STRIGI_IS_VERSION(2,1)
 * // Code for Strigi 2.1
 * #else
 * // Code for Strigi 2.0
 * #endif
 * \endcode
 */
#define STRIGI_IS_VERSION(a,b,c) ( STRIGI_VERSION >= STRIGI_MAKE_VERSION(a,b,c) )

/* use the same includes for types finding as in ConfigureChecks.cmake */
#if @STRIGI_HAVE_SOCKET_H@
#  include <socket.h>
#endif
#if @STRIGI_HAVE_SYS_SOCKET_H@
#  include <sys/socket.h>
#endif
#if @STRIGI_HAVE_SYS_TYPES_H@
#  include <sys/types.h>
#endif
#if @STRIGI_HAVE_UNISTD_H@
#  include <unistd.h>
#endif
#if @STRIGI_HAVE_STDINT_H@
#  include <stdint.h>
#endif
#if @STRIGI_HAVE_STDDEF_H@
#  include <stddef.h>
#endif

// our needed types
#if !@HAVE_INT8_T@
 #define HAVE_INT8_T 1
 #if ${SIZEOF_CHAR}==1 //is char one byte?
  typedef signed char int8_t;
 #else
  #error Could not determine type for int8_t!
 #endif
#endif

#if !@HAVE_UINT8_T@
 #define HAVE_UINT8_T 1
 #if ${SIZEOF_CHAR}==1 //is char one byte?
  typedef unsigned char uint8_t;
 #else
  #error Could not determine type for uint8_t!
 #endif
#endif

#if !@HAVE_INT16_T@
 #define HAVE_INT16_T 1
 #if ${SIZEOF_SHORT}==2 //is short two bytes?
  typedef short int16_t;
 #else
  #error Could not determine type for int16_t!
 #endif
#endif

#if !@HAVE_UINT16_T@
 #define HAVE_UINT16_T 1
 #if ${SIZEOF_SHORT}==2 //is short two bytes?
  typedef unsigned short uint16_t;
 #else
  #error Could not determine type for uint16_t!
 #endif
#endif

#if !@HAVE_INT32_T@
 #define HAVE_INT32_T 1
 #if ${SIZEOF_INT}==4 //is int four bytes?
  typedef int int32_t;
 #elif ${SIZEOF_LONG}==4 //is long four bytes?
  typedef long int32_t;
 #else
  #error Could not determine type for int32_t!
 #endif
#endif

#if !@HAVE_UINT32_T@
 #define HAVE_UINT32_T 1
 #if ${SIZEOF_INT}==4 //is int four bytes?
  typedef unsigned int uint32_t;
 #elif ${SIZEOF_LONG}==4 //is long four bytes?
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

#if !@HAVE_SIZE_T@
 #ifndef _SIZE_T_DEFINED 
  #ifndef HAVE_SIZE_T
   typedef unsigned int size_t;
   #define HAVE_SIZE_T 1
  #endif
  #define _SIZE_T_DEFINED 1     // kdewin32 define
 #endif
#endif

#cmakedefine __STRIGI_HAVE_GCC_VISIBILITY

/**
 * @def STRIGI_EXPORT
 *
 * The STRIGI_EXPORT macro marks the symbol of the given variable
 * to be visible, so it can be used from outside the resulting library.
 *
 */
#ifdef __STRIGI_HAVE_GCC_VISIBILITY
#define STRIGI_EXPORT __attribute__ ((visibility("default")))
#define STRIGI_IMPORT
#elif defined(_WIN32) || defined(_WIN64)
#define STRIGI_EXPORT __declspec(dllexport)
#define STRIGI_IMPORT __declspec(dllimport)
#else
#define STRIGI_EXPORT
#define STRIGI_IMPORT
#endif

#ifndef STREAMS_EXPORT
# ifdef MAKE_STREAMS_LIB
#  define STREAMS_EXPORT STRIGI_EXPORT
# else
#  define STREAMS_EXPORT STRIGI_IMPORT
# endif
#endif

#ifndef STREAMANALYZER_EXPORT
# ifdef MAKE_STREAMANALYZER_LIB
#  define STREAMANALYZER_EXPORT STRIGI_EXPORT
# else
#  define STREAMANALYZER_EXPORT STRIGI_IMPORT
# endif
#endif

#ifndef STRIGI_QTDBUSCLIENT_EXPORT
# ifdef MAKE_STRIGI_QTDBUSCLIENT_LIB
#  define STRIGI_QTDBUSCLIENT_EXPORT STRIGI_EXPORT
# else
#  define STRIGI_QTDBUSCLIENT_EXPORT STRIGI_IMPORT
# endif
#endif

#ifndef SEARCHCLIENT_EXPORT
# ifdef MAKE_SEARCHCLIENT_LIB
#  define SEARCHCLIENT_EXPORT STRIGI_EXPORT
# else
#  define SEARCHCLIENT_EXPORT STRIGI_IMPORT
# endif
#endif

#ifndef STRIGIHTMLGUI_EXPORT
# ifdef MAKE_STRIGIHTMLGUI_LIB
#  define STRIGIHTMLGUI_EXPORT STRIGI_EXPORT
# else
#  define STRIGIHTMLGUI_EXPORT STRIGI_IMPORT
# endif
#endif

#endif //STRIGICONFIG_H
