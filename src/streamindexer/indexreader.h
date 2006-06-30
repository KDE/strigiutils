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
#ifndef INDEXREADER_H
#define INDEXREADER_H

#include "indexeddocument.h"
#include <vector>
#include <set>

namespace jstreams {


/**
 * Break up a string in a query.
 * Currently very simple. Currently always combines terms with AND.
 **/
class Query {
private:
    int max;
    int offset;
    std::map<std::string, std::set<std::string> > includes;
    std::map<std::string, std::set<std::string> > excludes;

    const char* parseTerm(const char*);
public:
    Query(const std::string& q, int max, int offset);
    const std::map<std::string, std::set<std::string> > &getIncludes() const {
        return includes;
    }
    const std::map<std::string, std::set<std::string> > &getExcludes() const {
        return excludes;
    }
    int getMax() const { return max; }
    int getOffset() const { return offset; }
    std::string highlight(const std::string& text) const;
};

class IndexReader {
public:
    virtual ~IndexReader() {}
    virtual int32_t countHits(const Query&) = 0;
    virtual std::vector<IndexedDocument> query(const Query&) = 0;
    virtual std::map<std::string, time_t> getFiles(char depth) = 0;
    virtual int32_t countDocuments() { return -1; }
    virtual int32_t countWords() { return -1; }
    virtual int64_t getIndexSize() { return -1; }
};

}

#endif
