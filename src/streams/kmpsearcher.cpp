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
#include "strigiconfig.h"
#include "kmpsearcher.h"

using namespace std;
using namespace Strigi;

void
KmpSearcher::setQuery(const string& query) {
    m_query = query;
    len = query.length();
    const char* p = query.c_str();
    if (table) {
        if (len > maxlen) {
            table = (int32_t*)realloc(table, sizeof(int32_t)*(len+1));
            maxlen = len;
        }
    } else {
        table = (int32_t*)malloc(sizeof(int32_t)*(len+1));
        maxlen = len;
    }
    int32_t i = 0;
    int32_t j = -1;
    char c = '\0';

    // build the mismatch table
    table[0] = j;
    while (i < len) {
        if (p[i] == c) {
            table[i + 1] = j + 1;
            ++j;
            ++i;
        } else if (j > 0) {
            j = table[j];
        } else {
            table[i + 1] = 0;
            ++i;
            j = 0;
        }
        c = p[j];
    }
}
const char*
KmpSearcher::search(const char* haystack, int32_t haylen) const {
    if (table == 0) return 0;
//    printf("start search %i\n", len);
    const char* needle = m_query.c_str();
    // search for the pattern
    int32_t i = 0;
    int32_t j = 0;
    while (j + i < haylen && i < len) {
        if (haystack[j + i] == needle[i]) {
            ++i;
        } else {
            j += i - table[i];
            if (i > 0) i = table[i];
        }
    }

    if (needle[i] == '\0') {
        return haystack + j;
    } else {
        return 0;
    }
}

