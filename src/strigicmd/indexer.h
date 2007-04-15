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
#ifndef INDEXER_H
#define INDEXER_H

#include "indexmanager.h"
#include "streamanalyzer.h"

class FileLister;

class Indexer {
private:
    FileLister* lister;
    Strigi::IndexManager* manager;
    Strigi::IndexWriter* writer;
    const std::string indexdir;
    Strigi::AnalyzerConfiguration& config;
    Strigi::StreamAnalyzer* const analyzer;

    static void addFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime);
    static Indexer* workingIndexer;
    void doFile(const char* filepath);
public:
    Indexer(const std::string& indexdir, const std::string& backend,
        Strigi::AnalyzerConfiguration& ic);
    ~Indexer();
    void index(const char* dir);
};

#endif
