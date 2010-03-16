/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/kmpsearcher.h>
#include <strigi/strigiconfig.h>
#include <algorithm>

/* This is not the KMP algorigthm. We're now using the faster (turbo)
   Boyer-Moore algorithm:
     http://www-igm.univ-mlv.fr/~lecroq/string/node15.html
*/

using namespace std;
using namespace Strigi;

void
preBmBc(const char* x, int32_t m, int32_t* bmBc) {
    int32_t i;
 
    for (i = 0; i < 256; ++i) {
        bmBc[i] = 1;
    }
    for (i = 0; i < m - 1; ++i) {
        bmBc[(unsigned char)x[i]] = -i;
    }
}
void
suffixes(const char* x, int32_t m, int32_t* suff) {
    int32_t f, g, i;

    f = 0; 
    suff[m - 1] = m;
    g = m - 1;
    for (i = m - 2; i >= 0; --i) {
        if (i > g && suff[i + m - 1 - f] < i - g) {
            suff[i] = suff[i + m - 1 - f];
        } else {
            if (i < g) {
                g = i;
            }
            f = i;
            while (g >= 0 && x[g] == x[g + m - 1 - f]) {
                --g;
            }
            suff[i] = f - g;
        }
    }
}
void
preBmGs(const char* x, int32_t m, int32_t* bmGs) {
    int32_t i, j;
    int32_t* suff;

    suff = new int32_t[m];
 
    suffixes(x, m, suff);
 
    for (i = 0; i < m; ++i) {
        bmGs[i] = m;
    }
    j = 0;
    for (i = m - 1; i >= 0; --i) {
        if (suff[i] == i + 1) {
            for (; j < m - 1 - i; ++j) {
                if (bmGs[j] == m) {
                    bmGs[j] = m - 1 - i;
                }
            }
        }
    }
    for (i = 0; i <= m - 2; ++i) {
        bmGs[m - 1 - suff[i]] = m - 1 - i;
    }

    delete [] suff;
}
KmpSearcher::KmpSearcher(const std::string& query) :table(0) {
    setQuery(query);
}
void
KmpSearcher::setQuery(const string& query) {
    m_query = query;
    len = (int32_t)query.length();
    const char* p = query.c_str();
    int32_t tablesize = 4 * (257+len);
    if (table) {
        if (len > maxlen) {
            table = (int32_t*)realloc(table, tablesize);
            maxlen = len;
        }
    } else {
        table = (int32_t*)malloc(tablesize);
        maxlen = len;
    }

    preBmGs(p, len, table+256);
    preBmBc(p, len, table);
}
const char*
KmpSearcher::search(const char* haystack, int32_t haylen) const {
    if (table == 0) return 0;
    const char* needle = m_query.c_str();

    int32_t i = 0;
    int32_t *bmGs = table + 256;

    const char* hayend = haystack + haylen - len;
    const char* jp = haystack;

#ifndef TURBOBM
    int32_t bcShift, shift, u, v, turboShift;
    u = 0;
    shift = len;
    while (jp <= hayend) {
        i = len - 1;
        while (i >= 0 && needle[i] == jp[i]) {
            --i;
            if (u != 0 && i == len - 1 - shift) {
                i -= u;
            }
        }
        if (i < 0) {
            break;
        } else {
            v = len - 1 - i;
            turboShift = u - v;
            bcShift = table[(unsigned char)jp[i]] + i;
            shift = max(turboShift, bcShift);
            shift = max(shift, bmGs[i]);
            if (shift == bmGs[i]) {
                u = min(len - shift, v);
            } else {
                if (turboShift < bcShift) {
                    shift = max(shift, u + 1);
                }
                u = 0;
            }
        }
        jp += shift;
    }
#else
    while (jp <= hayend) {
        for (i = len - 1; i >= 0 && needle[i] == jp[i]; --i);
        if (i < 0) {
            break;
        } else {
            jp += max(bmGs[i], table[(unsigned char)jp[i]] + i);
        }
    }
#endif

    if (i == -1) {
        return jp;
    } else {
        return 0;
    }
}

