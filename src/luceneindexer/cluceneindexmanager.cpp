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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "cluceneindexmanager.h"
#include <strigi/strigiconfig.h>
#include <CLucene.h>
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <iostream>
#include <sys/types.h>
#include <time.h>
#include "timeofday.h"
#include "stgdirent.h" //our dirent compatibility header... uses native if available

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

Strigi::IndexManager*
createCLuceneIndexManager(const char* path) {
    return new CLuceneIndexManager(path);
}

int CLuceneIndexManager::numberOfManagers = 0;

CLuceneIndexManager::CLuceneIndexManager(const std::string& path)
        {//: bitsets(this) {
    ++numberOfManagers;
    dbdir = path;
    indexwriter = 0;
    writer = new CLuceneIndexWriter(this);
    analyzer = new StandardAnalyzer();
    mtime = 0;

    //remove any old segments lying around from crashes, etc
    //writer->cleanUp();
    // make sure there's at least an index
    openWriter();
}
CLuceneIndexManager::~CLuceneIndexManager() {
    // close the writer and analyzer
    delete writer;
    std::map<STRIGI_THREAD_TYPE, CLuceneIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
        r->second = 0;
    }
    closeWriter();
    delete analyzer;
    if (--numberOfManagers == 0) {
// temporarily commented out because of problem with clucene
//        _lucene_shutdown();
    }
}
Strigi::IndexReader*
CLuceneIndexManager::indexReader() {
    return luceneReader();
}
CLuceneIndexReader*
CLuceneIndexManager::luceneReader() {
    // TODO check if we should update/reopen the reader
    STRIGI_THREAD_TYPE self = STRIGI_THREAD_SELF();
    CLuceneIndexReader* r;
    STRIGI_MUTEX_LOCK(&lock.lock);
    r = readers[self];
    STRIGI_MUTEX_UNLOCK(&lock.lock);
    if (r == 0) {
        r = new CLuceneIndexReader(this, dbdir);
        STRIGI_MUTEX_LOCK(&lock.lock);
        readers[self] = r;
        STRIGI_MUTEX_UNLOCK(&lock.lock);
    }
    return r;
}
Strigi::IndexWriter*
CLuceneIndexManager::indexWriter() {
    return writer;
}
/*Strigi::QueryBitsetCache*
CLuceneIndexManager::bitSets() {
    return &bitsets;
}*/
IndexWriter*
CLuceneIndexManager::refWriter() {
    STRIGI_MUTEX_LOCK(&writelock.lock);
    if (indexwriter == 0) {
        openWriter();
    }
    return indexwriter;
}
void
CLuceneIndexManager::derefWriter() {
    STRIGI_MUTEX_UNLOCK(&writelock.lock);
}
void
CLuceneIndexManager::openWriter(bool truncate) {
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
    // close all readers, so they will be reopenened with the fresh data
    refreshReaders();
    try {
        indexwriter->close();
        delete indexwriter;
    } catch (CLuceneError& err) {
        printf("could not close writer: %s\n", err.what());
    }
    indexwriter = 0;
    // clear the cache
    //bitsets.clear();
}
void
CLuceneIndexManager::refreshReaders() {
    map<STRIGI_THREAD_TYPE, CLuceneIndexReader*>::iterator i;
    for (i = readers.begin(); i != readers.end(); ++i) {
        if (i->second) {
            i->second->closeReader();
        }
    }
}
int
CLuceneIndexManager::docCount() {
    return luceneReader()->reader->numDocs();
}
int64_t
CLuceneIndexManager::indexSize() {
    // sum the sizes of the files in the index
    // loop over directory entries
    DIR* dir = opendir(dbdir.c_str());
    if (dir == 0) {
        fprintf(stderr, "could not open index directory %s (%s)\n", dbdir.c_str(), strerror(errno));
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
            fprintf(stderr, "could not open file %s (%s)\n", filename.c_str(), strerror(errno));
        }
        e = readdir(dir);
    }
    closedir(dir);
    return size;
}
void
CLuceneIndexManager::deleteIndex() {
    closeWriter();
    refreshReaders();
    openWriter(true);
}
time_t
CLuceneIndexManager::indexMTime() {
    time_t t;
    STRIGI_MUTEX_LOCK(&lock.lock);
    t = mtime;
    STRIGI_MUTEX_UNLOCK(&lock.lock);
    return t;
}
void
CLuceneIndexManager::setIndexMTime() {
    struct timeval t;
    gettimeofday(&t, 0);
    STRIGI_MUTEX_LOCK(&lock.lock);
    mtime = t.tv_sec;
    STRIGI_MUTEX_UNLOCK(&lock.lock);
}
