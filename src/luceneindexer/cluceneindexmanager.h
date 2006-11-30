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
class CLuceneIndexManager : public jstreams::IndexManager {
private:
    StrigiMutex writelock;
    StrigiMutex lock;
    std::string dbdir;
    std::map<pthread_t, CLuceneIndexReader*> readers;
    CLuceneIndexWriter* writer;
    lucene::index::IndexWriter* indexwriter;
    //jstreams::QueryBitsetCache bitsets;
    lucene::analysis::Analyzer* analyzer;
    time_t mtime;
    static int numberOfManagers;

    void openWriter(bool truncate=false);
public:
    explicit CLuceneIndexManager(const std::string& path);
    ~CLuceneIndexManager();

    lucene::index::IndexWriter* refWriter();
    void derefWriter();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    CLuceneIndexReader* getReader();
//    jstreams::QueryBitsetCache* getBitSets();
    int32_t docCount();
    int64_t getIndexSize();
    void deleteIndex();
    void closeWriter();
    time_t getIndexMTime();
    void setIndexMTime();
};

jstreams::IndexManager*
createCLuceneIndexManager(const char* path);

std::string wchartoutf8(const wchar_t*);
std::wstring utf8toucs2(const char*);
std::string wchartoutf8(const std::wstring&);
std::wstring utf8toucs2(const std::string&);

#endif
