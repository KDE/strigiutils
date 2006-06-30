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
#ifndef SQLITEINDEXMANAGER_H
#define SQLITEINDEXMANAGER_H

#include "indexmanager.h"
#include <sqlite3.h>
#include <pthread.h>
#include <string>
#include <map>

/**
 * Manage the database connection between threads.
 * http://www.sqlite.org/cvstrac/wiki?p=MultiThreading
 **/

struct sqlite3;
class SqliteIndexReader;
class SqliteIndexWriter;
class SqliteIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbfile;
    std::map<pthread_t, SqliteIndexReader*> readers;
    std::map<pthread_t, SqliteIndexWriter*> writers;
    std::map<pthread_t, sqlite3*> dbs;

public:
    SqliteIndexManager(const char* path);
    ~SqliteIndexManager();

    sqlite3* ref();
    void deref();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    static std::string escapeSqlValue(const std::string& value);
    static sqlite3* opendb(const char*);
};
jstreams::IndexManager*
createSqliteIndexManager(const char* path);

#endif
