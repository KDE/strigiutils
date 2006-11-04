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
#include "xapianindexmanager.h"
#include "xapianindexreader.h"
#include "xapianindexwriter.h"
#include <assert.h>
using namespace std;
using namespace jstreams;
using namespace Xapian;

pthread_mutex_t XapianIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

jstreams::IndexManager*
createXapianIndexManager(const char* path) {
    return new XapianIndexManager(path);
}

XapianIndexManager::XapianIndexManager(const char* dbd)
        : dblock(lock), dbdir(dbd) {
    db = new WritableDatabase(dbdir.c_str(), DB_CREATE_OR_OPEN);
}
XapianIndexManager::~XapianIndexManager() {
    std::map<pthread_t, XapianIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, XapianIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
    closedb();
}
IndexReader*
XapianIndexManager::getIndexReader() {
    pthread_t self = pthread_self();
    XapianIndexReader* r = readers[self];
    if (r == 0) {
        r = new XapianIndexReader(this, db);
        readers[self] = r;
    }
    return r;
}
IndexWriter*
XapianIndexManager::getIndexWriter() {
    pthread_t self = pthread_self();
    XapianIndexWriter* w = writers[self];
    if (w == 0) {
        w = new XapianIndexWriter(this, db);
        writers[self] = w;
    }
    return w;
}
/*ESTDB*
XapianIndexManager::getWriteDB() {
    STRIGI_MUTEX_LOCK(&dblock);
    // check if a writable db is already opened
    if (db) {
        if (writing) return db;
        closedb();
    }
    writing = true;
    int errorcode;
    db = est_db_open(dbdir.c_str(), ESTDBWRITER|ESTDBCREAT, &errorcode);
    if (db == 0) printf("could not open db: error %i\n", errorcode);
    return db;
}
void
XapianIndexManager::returnWriteDB() {
    STRIGI_MUTEX_UNLOCK(&dblock);
}
ESTDB*
XapianIndexManager::getReadDB() {
    STRIGI_MUTEX_LOCK(&dblock);
    // check if a readable db is already opened
    if (db) {
        if (!writing) return db;
        closedb();
    }
    writing = false;
    return db;
}
void
XapianIndexManager::returnReadDB() {
    STRIGI_MUTEX_UNLOCK(&dblock);
}*/
void
XapianIndexManager::ref() {
    STRIGI_MUTEX_LOCK(&dblock);
}
void
XapianIndexManager::deref() {
    STRIGI_MUTEX_UNLOCK(&dblock);
}
void
XapianIndexManager::closedb() {
    if (db == 0) return;
    delete db;
    db = 0;
}
