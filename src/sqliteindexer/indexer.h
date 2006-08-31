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

#include <string>
#include "filelister.h"
#include "sqliteindexmanager.h"
#include "streamindexer.h"

class Indexer {
private:
    FileLister m_lister;
    const std::string m_indexdir;
    SqliteIndexManager m_manager;
    jstreams::StreamIndexer m_indexer;

    static bool addFileCallback(const std::string& path,
        const char *filename, time_t mtime);
    static Indexer *workingIndexer;
    void doFile(const std::string &filepath);
public:
    explicit Indexer(const char *indexdir);
    ~Indexer();
    void index(const char *dir);
};

#endif
