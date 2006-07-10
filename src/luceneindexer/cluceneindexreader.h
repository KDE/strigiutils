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
#ifndef CLUCENEINDEXREADER_H
#define CLUCENEINDEXREADER_H

#include "indexreader.h"
#include <map>

namespace lucene {
    namespace index {
        class Term;
    }
    namespace search {
        class BooleanQuery;
    }
    namespace document {
        class Document;
        class Field;
    }
}

class CLuceneIndexManager;
class CLuceneIndexReader : public jstreams::IndexReader {
friend class CLuceneIndexManager;
private:
    CLuceneIndexManager* manager;
    int countversion;
    int32_t count;

    CLuceneIndexReader(CLuceneIndexManager* m) :manager(m), countversion(-1) {}
    ~CLuceneIndexReader();

    static const char* mapId(const std::string& id);
    static lucene::index::Term* createTerm(const std::string& name,
        const std::string& value);
    static void createBooleanQuery(const jstreams::Query& query,
        lucene::search::BooleanQuery& bq);
    static std::string convertValue(const TCHAR* value);
    static void addField(lucene::document::Field* field,
        jstreams::IndexedDocument&);
public:
    int32_t countHits(const jstreams::Query&);
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int32_t countDocuments();
    int32_t countWords();
    int64_t getIndexSize();
};

#endif
