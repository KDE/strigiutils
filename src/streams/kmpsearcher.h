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
#ifndef KMPSEARCHER_H
#define KMPSEARCHER_H

#include <string>

namespace jstreams {
/**
 * Class for string search that uses the Knuth-Morris-Pratt algorithm.
 * Code based on the example on
 * http://en.wikipedia.org/wiki/Knuth-Morris-Pratt_algorithm
 **/
class KMPSearcher {
private:
    std::string query;
    int32_t* table;
    int32_t len;
    int32_t maxlen;
public:
    KMPSearcher() :table(0) { }
    ~KMPSearcher() {
        if (table) {
            free(table);
        }
    }
    void setQuery(const std::string& );
    int32_t getQueryLength() const { return len; }
    std::string getQuery() const { return query; }
    const char* search(const char* haystack, int32_t haylen) const;
};
}

#endif
