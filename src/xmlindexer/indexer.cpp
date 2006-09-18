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
#include "jstreamsconfig.h"
#include "indexer.h"
#include "filelister.h"
#include "filereader.h"
#include "filtermanager.h"

using namespace std;

Indexer *Indexer::workingIndexer;

Indexer::Indexer(FilterManager* filtermanager)
    :m_indexer(&writer)
{
    m_lister = new FileLister (filtermanager);
}

Indexer::~Indexer( )
{
    delete m_lister;
}
void
Indexer::index(const char *dir) {
    printf("<?xml version='1.0' encoding='UTF-8'?>\n<metadata>\n");
    workingIndexer = this;
    m_lister->setCallbackFunction(&Indexer::addFileCallback);
    bool exceptions = true;
    if (exceptions) {
        try {
            m_lister->listFiles(dir);
        } catch(...) {
            printf("Unknown error");
        }
    } else {
        m_lister->listFiles(dir);
    }
    printf("</metadata>\n");
}
bool
Indexer::addFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime) {
    workingIndexer->doFile(path);
    return true;
}
void
Indexer::doFile(const char* filepath) {
    m_indexer.indexFile(filepath);
}
