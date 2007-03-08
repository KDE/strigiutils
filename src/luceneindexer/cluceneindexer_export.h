/* This file is part of Strigi Desktop Search
 *
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

#ifndef CLUCENEINDEXER_EXPORT_H
#define CLUCENEINDEXER_EXPORT_H

#ifdef _WIN32
# ifdef MAKE_CLUCENEINDEXER_LIB
#  define CLUCENEINDEXER_EXPORT __declspec(dllexport)
# else
#  define CLUCENEINDEXER_EXPORT __declspec(dllimport)
# endif
#else
# define CLUCENEINDEXER_EXPORT
#endif

#endif  // CLUCENEINDEXER_EXPORT_H