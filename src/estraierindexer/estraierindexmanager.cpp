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
#include "estraierindexmanager.h"
#include <strigi/strigiconfig.h>
#include "estraierindexreader.h"
#include "estraierindexwriter.h"
#include "strigi_thread.h"
#include "indexplugin.h"
#include <iostream>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "stgdirent.h" //our dirent compatibility header... uses native if available
using namespace std;
using namespace Strigi;

/* define and export the index factory */
REGISTER_STRIGI_INDEXMANAGER(EstraierIndexManager)

pthread_mutex_t EstraierIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

EstraierIndexManager::EstraierIndexManager(const char* dbd)
        : dblock(lock), dbdir(dbd) {
    int errorcode;
    db = est_db_open(dbdir.c_str(),
        ESTDBCREAT|ESTDBWRITER|ESTDBREADER|ESTDBNOLCK|ESTDBPERFNG,
        &errorcode);
    if (db == 0) printf("could not open db %s: %s\n", dbdir.c_str(),
        est_err_msg(errorcode));
}
EstraierIndexManager::~EstraierIndexManager() {
    std::map<pthread_t, EstraierIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, EstraierIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
    closedb();
}
IndexReader*
EstraierIndexManager::indexReader() {
    pthread_t self = pthread_self();
    EstraierIndexReader* r = readers[self];
    if (r == 0) {
        r = new EstraierIndexReader(this);
        readers[self] = r;
    }
    return r;
}
IndexWriter*
EstraierIndexManager::indexWriter() {
    pthread_t self = pthread_self();
    EstraierIndexWriter* w = writers[self];
    if (w == 0) {
        w = new EstraierIndexWriter(this);
        writers[self] = w;
    }
    return w;
}
ESTDB*
EstraierIndexManager::ref() {
    STRIGI_MUTEX_LOCK(&dblock);
    return db;
}
void
EstraierIndexManager::deref() {
    STRIGI_MUTEX_UNLOCK(&dblock);
}
void
EstraierIndexManager::closedb() {
    if (db == 0) return;
    int errorcode;
    int ok = est_db_close(db, &errorcode);
    if (!ok) printf("could not close db: error %i\n", errorcode);
    db = 0;
}
void
removefiles(const string& d, bool rmd = false) {
    // remove all entries from the subdir
    DIR* dir = opendir(d.c_str());
    if (dir == 0) {
        fprintf(stderr, "could not open index directory %s (%s)\n", d.c_str(), strerror(errno));
        return;
    }
    // delete all the index files
    struct dirent* e = readdir(dir);
    while (e != 0) {
        // skip the directories '.' and '..'
        char c1 = e->d_name[0];
        if (c1 == '.') {
            char c2 = e->d_name[1];
            if (c2 == '.' || c2 == '\0') {
                e = readdir(dir);
                continue;
            }
        }
        string filename = d+'/'+e->d_name;
        struct stat s;
        // use lstat, we don't want to follow into symlinked directories
        int r = lstat(filename.c_str(), &s);
        if (r == 0) {
            if (S_ISDIR(s.st_mode)) {
                removefiles(filename, true);
            } else {
//                printf("unlink %s\n", filename.c_str());
                unlink(filename.c_str());
            }
        } else {
            fprintf(stderr, "could not open file %s (%s)\n", filename.c_str(), strerror(errno));
        }
        e = readdir(dir);
    }
    closedir(dir);
    if (rmd) {
        rmdir(d.c_str());
    }
}
void
EstraierIndexManager::deleteIndex() {
    ref();
    closedb();

    removefiles(dbdir);
    // open the index again
    int errorcode;
    db = est_db_open(dbdir.c_str(), ESTDBCREAT|ESTDBWRITER|ESTDBREADER|ESTDBNOLCK|ESTDBPERFNG, &errorcode);
    if (db == 0) printf("could not reopen db %s: %s\n", dbdir.c_str(),
        est_err_msg(errorcode));
    deref();
}
