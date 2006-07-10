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
#include "querybitset.h"
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
    StrigiMutex* dblock;
    static StrigiMutex lock;
    std::string dbdir;
    CLuceneIndexReader* reader;
    CLuceneIndexWriter* writer;
    lucene::index::IndexWriter* indexwriter;
    lucene::index::IndexReader* indexreader;
    jstreams::QueryBitsetCache bitsets;
    lucene::analysis::Analyzer* analyzer;
    int version;
    static int numberOfManagers;

    void openReader();
    void closeReader();
    void openWriter();
    void closeWriter();
public:
    CLuceneIndexManager(const std::string& path);
    ~CLuceneIndexManager();

    lucene::index::IndexWriter* refWriter();
    void derefWriter();
    lucene::index::IndexReader* refReader();
    void derefReader();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    jstreams::QueryBitsetCache* getBitSets();
    int docCount();
    int getIndexSize();
    int getVersion() const { return version; }
};

jstreams::IndexManager*
createCLuceneIndexManager(const char* path);

#endif
