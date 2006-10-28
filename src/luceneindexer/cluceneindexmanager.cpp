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
#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <CLucene.h>
#include <sys/types.h>
#include "stgdirent.h" //our dirent compatibility header... uses native if available

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

StrigiMutex CLuceneIndexManager::lock;

jstreams::IndexManager*
createCLuceneIndexManager(const char* path) {
    return new CLuceneIndexManager(path);
}

int CLuceneIndexManager::numberOfManagers = 0;

CLuceneIndexManager::CLuceneIndexManager(const std::string& path)
        {//: bitsets(this) {
    ++numberOfManagers;
    dblock = &lock;
    dbdir = path;
    indexreader = 0;
    indexwriter = 0;
    version = 0;
    writer = new CLuceneIndexWriter(this);
    reader = new CLuceneIndexReader(this);
    analyzer = new StandardAnalyzer();
    // make sure there's at least an index
    openWriter();
}
CLuceneIndexManager::~CLuceneIndexManager() {
    // close the writer and analyzer
    delete writer;
    delete reader;
    closeReader();
    closeWriter();
    delete analyzer;
    if (--numberOfManagers == 0) {
// temporarily commented out because of problem with clucene
//        _lucene_shutdown();
    }
}
jstreams::IndexReader*
CLuceneIndexManager::getIndexReader() {
    return reader;
}
jstreams::IndexWriter*
CLuceneIndexManager::getIndexWriter() {
    return writer;
}
/*jstreams::QueryBitsetCache*
CLuceneIndexManager::getBitSets() {
    return &bitsets;
}*/
IndexWriter*
CLuceneIndexManager::refWriter() {
    STRIGI_MUTEX_LOCK(&dblock->lock);
    if (indexwriter == 0) {
        closeReader();
        openWriter();
    }
    return indexwriter;
}
void
CLuceneIndexManager::derefWriter() {
    STRIGI_MUTEX_UNLOCK(&dblock->lock);
}
IndexReader*
CLuceneIndexManager::refReader() {
    STRIGI_MUTEX_LOCK(&dblock->lock);
    if (indexreader == 0) {
        closeWriter();
        openReader();
    }
    return indexreader;
}
void
CLuceneIndexManager::derefReader() {
    STRIGI_MUTEX_UNLOCK(&dblock->lock);
}
void
CLuceneIndexManager::openReader() {
    try {
//        printf("reader at %s\n", dbdir.c_str());
        indexreader = IndexReader::open(dbdir.c_str());
    } catch (CLuceneError& err) {
        printf("could not create reader: %s\n", err.what());
    }
}
void
CLuceneIndexManager::closeReader() {
    if (indexreader == 0) return;
    try {
        indexreader->close();
    } catch (CLuceneError& err) {
        printf("could not close clucene: %s\n", err.what());
    }
    delete indexreader;
    indexreader = 0;
}
void
CLuceneIndexManager::openWriter(bool truncate) {
    version++;
    try {
        if (!truncate && IndexReader::indexExists(dbdir.c_str())) {
            if (IndexReader::isLocked(dbdir.c_str())) {
                IndexReader::unlock(dbdir.c_str());
            }
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, false);
        } else {
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, true, true);
        }
    } catch (CLuceneError& err) {
        printf("could not create writer: %s\n", err.what());
    }
}
void
CLuceneIndexManager::closeWriter() {
    if (indexwriter == 0) return;
    indexwriter->close();
    delete indexwriter;
    indexwriter = 0;
    // clear the cache
    //bitsets.clear();
}
int
CLuceneIndexManager::docCount() {
    int count;
    STRIGI_MUTEX_LOCK(&dblock->lock);
    if (indexwriter) {
        count = indexwriter->docCount();
    } else {
        if (indexreader == 0) {
            openReader();
        }
        count = indexreader->numDocs();
    }
    STRIGI_MUTEX_UNLOCK(&dblock->lock);
    return count;
}
int64_t
CLuceneIndexManager::getIndexSize() {
    // sum the sizes of the files in the index
    // loop over directory entries
    DIR* dir = opendir(dbdir.c_str());
    if (dir == 0) {
        fprintf(stderr, "could not open index directory.\n");
        return -1;
    }
    struct dirent* e = readdir(dir);
    int64_t size = 0;
    while (e != 0) {
        string filename = dbdir+'/'+e->d_name;
        struct stat s;
        int r = stat(filename.c_str(), &s);
        if (r == 0) {
            if ( S_ISREG(s.st_mode)) {
                size += s.st_size;
            }
        } else {
            fprintf(stderr, "could not open file %s\n", filename.c_str());
        }
        e = readdir(dir);
    }
    closedir(dir);
    return size;
}
void
CLuceneIndexManager::deleteIndex() {
    closeReader();
    closeWriter();
    openWriter(true);
}
