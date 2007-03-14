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
#ifndef INDEXER_H
#define INDEXER_H

#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "streamanalyzer.h"

class FileLister;

class Indexer {
private:
    FileLister* m_lister;
    const std::string m_indexdir;
    CLuceneIndexManager m_manager;
    CLuceneIndexWriter m_writer;
    Strigi::StreamAnalyzer m_indexer;

    static void addFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime);
    static Indexer *workingIndexer;
    void doFile(const char* filepath);
public:
    Indexer(const char *indexdir, Strigi::AnalyzerConfiguration& ic);
    ~Indexer();
    void index(const char *dir);
};

#endif
