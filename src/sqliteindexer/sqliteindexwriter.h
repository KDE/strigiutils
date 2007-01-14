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
#ifndef SQLITEINDEXWRITER_H
#define SQLITEINDEXWRITER_H

#include "indexwriter.h"
#include <map>

struct sqlite3;
struct sqlite3_stmt;
class SqliteIndexManager;

class SqliteIndexWriter : public jstreams::IndexWriter {
friend class SqliteIndexManager;
private:
    sqlite3* dbcheck;
    SqliteIndexManager* manager;
    sqlite3_stmt *insertvaluestmt, *getfilestmt, *updatefilestmt,
        *insertfilestmt;
    const std::string indexpath;
    std::map<int64_t, std::map<std::string, int> > content;

    int temprows;
    void prepareStmt(sqlite3* db, sqlite3_stmt*& stmt, const char* sql,
        int sqllength);
    void finalizeStmt(sqlite3* db, sqlite3_stmt*& stmt);
protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length);
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const unsigned char* data, int32_t size) {}
    SqliteIndexWriter(SqliteIndexManager*, sqlite3*);
    ~SqliteIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteAllEntries();
    int itemsInCache() { return temprows; };
};

#endif
