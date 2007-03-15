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
#ifndef CNSTR_H
#define CNSTR_H

#include <map>
#include "streamanalyzer_export.h"

/**
 * This class is a memory saving, performance increasing class for working
 * with constant strings. Equal strings only take up memory
 * once. String comparison is simply pointer comparison.
 * This comes at the cost of maintaining an index. This cost manifests itself
 * only at the time of creation.
 * Right now we use stl for implementing the tree, but the implementation
 * could be much faster with a map specifically for strings. A good implementation
 * might be to use a Patricia trie.
 * http://www.codeproject.com/string/PatriciaTrieTemplateClass.asp
 * http://cr.yp.to/critbit.html
 * http://goog-sparsehash.sourceforge.net/
 *
 * Each object has the size of only an iterator which is the same size as
 * pointer to a character would be.
 **/
class STREAMANALYZER_EXPORT cnstr {
private:
    /* comparator for determining if s1 < s2 */
    struct ltstr {
        bool operator() (const char *s1, const char *s2) const {
            return (s1 == 0) ?s2 != 0 : s2 != 0 && strcmp(s1, s2) < 0;
        }
    };
    // static version of default object to avoid a lookup call when
    // creating an unintialized object
    std::map<const char *, int, ltstr>& strings() {
        static std::map<const char *, int, ltstr> s;
        return s;
    }
    std::map<const char *, int, ltstr>::iterator pos;

    void init(const char *s) {
        // check if this string is already in memory
        pos = strings().find(s);
        if (pos == strings().end()) {
            // create a copy
            if (s) {
                int l = strlen(s);
                char *c = new char[l + 1];
                strcpy(c, s);
                s = c;
            }
            pos = strings().insert(std::make_pair<const char *, int>(s, 1))
                .first;
        } else {   // otherwise, just link to it
            pos->second++;
        }
    }
    void clear() {
        // decrease reference counter
        if (--(pos->second) == 0) {
            if (pos->first) {
                delete [] pos->first;
            }
            strings().erase(pos);
        }
    }
    void copy(const cnstr & c) {
        pos = c.pos;
        pos->second++;
    }
public:
    static cnstr empty;
    static cnstr null;
    cnstr(const char* c = 0) {
        init(c);
    }
    cnstr(const cnstr& c) {
        copy(c);
    }
    ~cnstr() {
        clear();
    }
    cnstr& operator=(const cnstr& c) {
        clear();
        copy(c);
        return* this;
    }
    cnstr& operator=(const char* c) {
        clear();
        init(c);
        return* this;
    }
    /* This function sorts, but not alphabetically.
     * The sorting should be stable, because for a constant object
     * the value pos->first will stay constant. It's also very quick,
     * because we simply compare pointers. We do not compare iterators,
     * because they are bidirectional and < is expensive for them.
     */
    bool operator<(const cnstr& c) const {
        return pos->first < c.pos->first;
    }
    bool operator==(const cnstr& c) const {
        return pos == c.pos;
    }
    operator const char*() const {
        return pos->first;
    }
    const char *c_str() const {
        return pos->first;
    }
    bool isInitialized() const {
        return (pos->first != NULL);
    }
    size_t length() const {
        return (pos->first) ? strlen(pos->first) :0;
    }
};

#endif
