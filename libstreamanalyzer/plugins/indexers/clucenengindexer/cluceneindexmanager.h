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

#include <strigi/strigiconfig.h>
#include <strigi/indexmanager.h>
#include <strigi/strigi_thread.h>
#include <string>
#include <map>
#include <stdio.h>

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
        class Directory;
    }
}

class CLuceneIndexReader;
class CLuceneIndexWriter;
class CLuceneIndexManager : public Strigi::IndexManager {
private:
    std::string dbdir;
    CLuceneIndexReader* reader;
    CLuceneIndexWriter* writer;
    lucene::analysis::Analyzer* analyzer;
    static int numberOfManagers;

    lucene::index::IndexWriter* indexwriter;
    StrigiMutex writelock;
    int openWriters;
    void openWriter(bool truncate=false);

    lucene::index::IndexReader* indexreader;
    StrigiMutex readlock;
    struct timeval otime;
    int32_t wordcount;
    int32_t doccount;
    void openReader();
    void closeReader();
public:
    lucene::store::Directory* directory;

    explicit CLuceneIndexManager(const std::string& path);
    ~CLuceneIndexManager();

    lucene::index::IndexWriter* refWriter();
    void derefWriter();
    lucene::index::IndexReader* checkReader(bool ensureCurrent = false);

    Strigi::IndexReader* indexReader();
    Strigi::IndexWriter* indexWriter();
    CLuceneIndexReader* luceneReader();
    int64_t indexSize();
    int32_t countDocuments();
    int32_t countWords();
    void closeWriter();
};

#endif
