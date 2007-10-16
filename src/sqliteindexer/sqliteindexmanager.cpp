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
#include "sqliteindexmanager.h"
#include "sqliteindexreader.h"
#include "sqliteindexwriter.h"
#include "strigi_thread.h"
#include "indexplugin.h"
using namespace std;
using namespace Strigi;

/* define and export the index factory */
REGISTER_STRIGI_INDEXMANAGER(SqliteIndexManager)

pthread_mutex_t SqliteIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

Strigi::IndexManager*
createSqliteIndexManager(const char* path) {
    return new SqliteIndexManager(path);
}

SqliteIndexManager::SqliteIndexManager(const char* dbfile) {
    dblock = lock;
    this->dbfile = dbfile;
}
SqliteIndexManager::~SqliteIndexManager() {
    std::map<pthread_t, SqliteIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, SqliteIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
    std::map<pthread_t, sqlite3*>::iterator d;
    for (d = dbs.begin(); d != dbs.end(); ++d) {
        sqlite3_close(d->second);
    }
}
sqlite3*
SqliteIndexManager::opendb(const char* path) {
    sqlite3* db;
    int r = sqlite3_open(path, &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        fprintf(stderr, "could not open db %s: %s\n", path, sqlite3_errmsg(db));
        return 0;
    }
    // speed up by being unsafe and keeping temp tables in memory
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF;"
        "PRAGMA auto_vacuum = 1;"
        "PRAGMA temp_store = MEMORY;", 0, 0, 0);
    if (r != SQLITE_OK) {
        fprintf(stderr, "could not speed up database\n");
    }
    // create the tables required
    const char* sql;
    sql ="create table files (fileid integer primary key, "
        "path text, mtime integer, size integer, depth integer,"
        "unique (path));"
        "create index files_mtime on files(mtime);"
        "create index files_size on files(size);"
        "create table idx (fileid integer, name text, value, "
        "unique (fileid, name, value) on conflict ignore);"
        "create index idx_fileid on idx(fileid);"
        "create index idx_name on idx(name);"
        "create index idx_value on idx(value);"
        "create table words (wordid integer primary key, "
        "    word, count, unique(word));"
        "create table filewords (fileid integer, wordid integer, count,"
        "unique (fileid, wordid));"
        "create index filewords_wordid on filewords(wordid);"
;
    r = sqlite3_exec(db, sql, 0, 0, 0);
    if (r != SQLITE_OK) {
        fprintf(stderr, "could not create table %i %s\n", r,
            sqlite3_errmsg(db));
        exit(1);
    }

    // create temporary tables
    sql = "create temp table tempidx (fileid integer, name text, value);"
        "create temp table tempfilewords (fileid integer, word text, "
            "count integer); "
        "create index tempfilewords_word on tempfilewords(word);"
        "create index tempfilewords_fileid on tempfilewords(fileid);"
        // "begin immediate transaction;"
        ;
    r = sqlite3_exec(db, sql, 0,0,0);
    if (r != SQLITE_OK) {
        fprintf(stderr, "could not init writer: %i %s\n", r,
            sqlite3_errmsg(db));
    }
    return db;
}
sqlite3*
SqliteIndexManager::ref() {
    STRIGI_MUTEX_LOCK(&dblock);
    pthread_t self = pthread_self();
    sqlite3* db = dbs[self];
    if (db == 0) {
        db = opendb(dbfile.c_str());
        if (db) {
            dbs[self] = db;
        }
    }
    return db;
}
void
SqliteIndexManager::deref() {
    STRIGI_MUTEX_UNLOCK(&dblock);
}
IndexReader*
SqliteIndexManager::indexReader() {
    pthread_t self = pthread_self();
    SqliteIndexReader* r = readers[self];
    if (r == 0) {
        sqlite3* db = ref();
        if (db) {
            r = new SqliteIndexReader(this);
            readers[self] = r;
        }
        deref();
    }
    return r;
}
IndexWriter*
SqliteIndexManager::indexWriter() {
    pthread_t self = pthread_self();
    SqliteIndexWriter* w = writers[self];
    if (w == 0) {
        sqlite3* db = ref();
        if (db) {
            w = new SqliteIndexWriter(this, db);
            writers[self] = w;
        }
        deref();
    }
    return w;
}
string
SqliteIndexManager::escapeSqlValue(const string& value) {
    string v = value;
    // replace ' by ''
    size_t p = v.find('\'');
    while (p != string::npos) {
        v.replace(p, 1, "''");
        p = v.find('\'', p+2);
    }
    return v;
}
