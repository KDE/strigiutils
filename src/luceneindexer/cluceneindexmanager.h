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
#ifndef CLUCENEINDEXMANAGER_H
#define CLUCENEINDEXMANAGER_H

#include "strigiconfig.h"
#include "indexmanager.h"
//#include "querybitset.h"
#include <strigi_thread.h>
#include <string>
#include <map>

/**
 **/

namespace lucene {
    namespace analysis {
        class Analyzer;
    }
    namespace index {
        class IndexWriter;
        class IndexReader;
    }
    namespace store {
        class FSDirectory;
    }
}

class CLuceneIndexReader;
class CLuceneIndexWriter;
class CLUCENEINDEXER_EXPORT CLuceneIndexManager : public Strigi::IndexManager {
private:
    StrigiMutex writelock;
    StrigiMutex lock;
    std::string dbdir;
    std::map<STRIGI_THREAD_TYPE, CLuceneIndexReader*> readers;
    CLuceneIndexWriter* writer;
    lucene::index::IndexWriter* indexwriter;
    //Strigi::QueryBitsetCache bitsets;
    lucene::analysis::Analyzer* analyzer;
    time_t mtime;
    static int numberOfManagers;

    void openWriter(bool truncate=false);
public:
    explicit CLuceneIndexManager(const std::string& path);
    ~CLuceneIndexManager();

    lucene::index::IndexWriter* refWriter();
    void derefWriter();
    Strigi::IndexReader* indexReader();
    Strigi::IndexWriter* indexWriter();
    CLuceneIndexReader* luceneReader();
//    Strigi::QueryBitsetCache* bitSets();
    int32_t docCount();
    int64_t indexSize();
    void deleteIndex();
    void closeWriter();
    time_t indexMTime();
    void setIndexMTime();
};

CLUCENEINDEXER_EXPORT Strigi::IndexManager*
createCLuceneIndexManager(const char* path);

CLUCENEINDEXER_EXPORT std::string wchartoutf8(const wchar_t*);
CLUCENEINDEXER_EXPORT std::wstring utf8toucs2(const char*);
CLUCENEINDEXER_EXPORT std::string wchartoutf8(const std::wstring&);
CLUCENEINDEXER_EXPORT std::wstring utf8toucs2(const std::string&);

#endif
