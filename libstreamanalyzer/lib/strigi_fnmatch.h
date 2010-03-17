/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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

#ifndef STRIGI_FNMATCH_H
#define STRIGI_FNMATCH_H

#if defined(HAVE_FNMATCH_H)

#elif !defined(STRIGI_FNMATCH_H_)
#include <strigi/strigiconfig.h>
    #define STRIGI_FNMATCH_H


    #define FNM_NOMATCH     1     /**< Match failed. */
    #define FNM_NOESCAPE    0x01  /**< Disable backslash escaping. */
    #define FNM_PATHNAME    0x02  /**< Slash must be matched by slash. */
    #define FNM_PERIOD      0x04  /**< Period must be matched by period. */
    #define FNM_CASE_BLIND  0x08  /**< Compare characters case-insensitively.
                                       * @remark This flag is an Apache addition
                                       */
    #define SUCCESS 0

    int fnmatch(const char *pattern, const char *string, int flags);
#endif

#endif
