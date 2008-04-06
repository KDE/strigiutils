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
#include "indexplugin.h"
#include <iostream>
#include <sys/types.h>
#include <time.h>
#include "timeofday.h"
#include "stgdirent.h" //our dirent compatibility header... uses native if available


/* define and export the index factory */
REGISTER_STRIGI_INDEXMANAGER(CLuceneIndexManager)

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
    if (path == ":memory:") {
        ramdirectory = new lucene::store::RAMDirectory();
    } else {
        ramdirectory = 0;
    }
    gettimeofday(&mtime, 0);

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
    delete ramdirectory;
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
    lock.lock();
    r = readers[self];
    lock.unlock();
    if (r == 0) {
        r = new CLuceneIndexReader(this, dbdir);
        lock.lock();
        readers[self] = r;
        lock.unlock();
    }
    return r;
}
Strigi::IndexWriter*
CLuceneIndexManager::indexWriter() {
    return writer;
}
IndexWriter*
CLuceneIndexManager::refWriter() {
    writelock.lock();
    if (indexwriter == 0) {
        openWriter();
    }
    return indexwriter;
}
void
CLuceneIndexManager::derefWriter() {
    writelock.unlock();
}
void
CLuceneIndexManager::openWriter(bool truncate) {
    try {
        if (ramdirectory) {
            indexwriter = new IndexWriter(ramdirectory, analyzer, true);
        } else if (!truncate && IndexReader::indexExists(dbdir.c_str())) {
            if (IndexReader::isLocked(dbdir.c_str())) {
                IndexReader::unlock(dbdir.c_str());
            }
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, false);
        } else {
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, true);
        }
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not create writer: %s\n", err.what());
        indexwriter = 0;
    }
}
void
CLuceneIndexManager::closeWriter() {
    refWriter();
    if (indexwriter == 0) {
        derefWriter();
        return;
    }
    // update the timestamp on the index, so that the readers will reopen
    try {
        indexwriter->close();
        delete indexwriter;
    } catch (CLuceneError& err) {
        printf("could not close writer: %s\n", err.what());
    }
    indexwriter = 0;
    // clear the cache
    //bitsets.clear();
    derefWriter();
    setIndexMTime();
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
    setIndexMTime();
    openWriter(true);
}
struct timeval
CLuceneIndexManager::indexMTime() {
    struct timeval t;
    lock.lock();
    t = mtime;
    lock.unlock();
    return t;
}
void
CLuceneIndexManager::setIndexMTime() {
    lock.lock();
    gettimeofday(&mtime, 0);
    lock.unlock();
}

