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
#include <CLucene.h>
#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "stgdirent.h" //our dirent compatibility header... uses native if available

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

jstreams::IndexManager*
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
    std::map<pthread_t, CLuceneIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    closeWriter();
    delete analyzer;
    if (--numberOfManagers == 0) {
// temporarily commented out because of problem with clucene
//        _lucene_shutdown();
    }
}
jstreams::IndexReader*
CLuceneIndexManager::getIndexReader() {
    return getReader();
}
CLuceneIndexReader*
CLuceneIndexManager::getReader() {
    // TODO check if we should update/reopen the reader
    pthread_t self = pthread_self();
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
    indexwriter->close();
    delete indexwriter;
    indexwriter = 0;
    // clear the cache
    //bitsets.clear();
}
int
CLuceneIndexManager::docCount() {
    return getReader()->reader->numDocs();
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
    // todo: close all readers
    closeWriter();
    openWriter(true);
}
time_t
CLuceneIndexManager::getIndexMTime() {
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
std::wstring
utf8toucs2(const char*p, const char*e) {
    wstring ucs2;
    ucs2.reserve(3*(e-p));
    wchar_t w = 0;
    char nb = 0;
    while (p < e) {
        char c = *p;
        if (nb--) {
            w = (w<<6) + (c & 0x3f);
        } else if ((0xE0 & c) == 0xC0) {
            w = c & 0x1F;
            nb = 0;
        } else if ((0xF0 & c) == 0xE0) {
            w = c & 0x0F;
            nb = 1;
        } else if ((0xF8 & c) == 0xF0) {
            w = c & 0x07;
            nb = 2;
        } else {
            w = (w<<6) + (c&0x7F);
            ucs2 += w;
            w = 0;
            nb = 0;
        }
        p++;
    }
    return ucs2;
}
std::wstring
utf8toucs2(const char* p) {
    return utf8toucs2(p, p+strlen(p));
}
std::wstring
utf8toucs2(const std::string& utf8) {
    const char* p = utf8.c_str();
    const char* e = p + utf8.length();
    return utf8toucs2(p, e);
}
std::string
wchartoutf8(const wchar_t* p, const wchar_t* e) {
    string utf8;
    utf8.reserve((int)(1.5*(e-p)));
    while (p < e) {
        wchar_t c = *p;
        if (c < 0x80) {
            utf8 += (char)c;
        } else if (c < 0x800) {
            char c2 = (char)((c & 0x3f) | 0x80);
            utf8 += (c>>6) | 0xc0;
            utf8 += c2;
        } else if (c < 0x10000) {
            char c3 = (char)((c & 0x3f) | 0x80);
            char c2 = (char)(((c>>6) & 0x3f) | 0x80);
            utf8 += (c>>12) | 0xe0;
            utf8 += c2;
            utf8 += c3;
        }
        p++;
    }
    return utf8;
}
std::string
wchartoutf8(const wchar_t* p) {
    return wchartoutf8(p, p+wcslen(p));
}
std::string
wchartoutf8(const std::wstring& wchar) {
    const wchar_t *p = wchar.c_str();
    const wchar_t *e = p+wchar.length();
    return wchartoutf8(p, e);
}
