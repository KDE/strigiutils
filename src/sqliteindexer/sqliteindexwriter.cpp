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
#include "sqliteindexwriter.h"
#include "sqliteindexmanager.h"
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;
using namespace jstreams;

SqliteIndexWriter::SqliteIndexWriter(SqliteIndexManager *m)
        : manager(m) {
    temprows = 0;

    // prepare the sql statements
    const char* sql;
    sqlite3* db = manager->ref();
    dbcheck = db;
    sql = "insert into tempidx (fileid, name, value) values(?, ?, ?)";
    prepareStmt(db, insertvaluestmt, sql, strlen(sql));
    sql = "select fileid, mtime from files where path = ?;";
    prepareStmt(db, getfilestmt, sql, strlen(sql));
    sql = "update files set mtime = ?;";
    prepareStmt(db, updatefilestmt, sql, strlen(sql));
    sql = "insert into files (path, mtime, depth) values(?, ?, ?);'";
    prepareStmt(db, insertfilestmt, sql, strlen(sql));
    printf("opened db %p %p\n", db, insertfilestmt);
    manager->deref();
}
SqliteIndexWriter::~SqliteIndexWriter() {
    commit();
    sqlite3* db = manager->ref();
    finalizeStmt(db, insertvaluestmt);
    finalizeStmt(db, getfilestmt);
    finalizeStmt(db, updatefilestmt);
    finalizeStmt(db, insertfilestmt);
    manager->deref();
}
void
SqliteIndexWriter::prepareStmt(sqlite3* db, sqlite3_stmt*& stmt,
        const char* sql, int sqllength) {
    int r = sqlite3_prepare(db, sql, sqllength,& stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare statement '%s': %s\n", sql,
            sqlite3_errmsg(db));
        stmt = 0;
    }
}
void
SqliteIndexWriter::finalizeStmt(sqlite3* db, sqlite3_stmt*& stmt) {
    if (stmt) {
        int r = sqlite3_finalize(stmt);
        stmt = 0;
        if (r != SQLITE_OK) {
            printf("could not prepare statement: %s\n", sqlite3_errmsg(db));
        }
    }
}
void
SqliteIndexWriter::addText(const Indexable* idx, const char* text,
        int32_t length) {
    // very simple algorithm to get out sequences of ascii characters
    // we actually miss characters that are not on the edge between reads
    int64_t id = idx->getId();
    map<int64_t, map<string, int> >::iterator m = content.find(id);
    if (m == content.end()) {
        content[id];
        m = content.find(id);
    }
    map<string, int>* words = &m->second;

    const char* end = text + length;
    const char* p = text;
    while (p != end) {
        // find the start of a word
        while (p < end && !isalpha(*p)) ++p;
        if (p != end) {
            const char* e = p + 1;
            while (e < end && isalpha(*e)) ++e;
            if (e != end && e-p > 2 && e-p < 30) {
                std::string field(p, e-p);
                map<string, int>::iterator i = words->find(field);
                if (i == m->second.end()) {
                    (*words)[field] = 1;
                } else {
                    i->second++;
                }
            }
            p = e;
        }
    }
}
void
SqliteIndexWriter::setField(const Indexable* idx, const string &fieldname,
        const string& value) {
    int64_t id = idx->getId();
    //id = -1; // debug
    if (id == -1) return;
    sqlite3* db = manager->ref();
    assert(db == dbcheck);
    sqlite3_bind_int64(insertvaluestmt, 1, idx->getId());
    sqlite3_bind_text(insertvaluestmt, 2, fieldname.c_str(),
        fieldname.length(), SQLITE_STATIC);
    sqlite3_bind_text(insertvaluestmt, 3, value.c_str(), value.length(),
        SQLITE_STATIC);
    int r = sqlite3_step(insertvaluestmt);
    if (r != SQLITE_DONE) {
        printf("could not write into database: %i %s\n", r, sqlite3_errmsg(db));
        exit(1);
    }
    r = sqlite3_reset(insertvaluestmt);
    if (r != SQLITE_OK) {
        printf("could not reset statement: %i %s\n", r, sqlite3_errmsg(db));
    }
    temprows++;
    manager->deref();
}
void
SqliteIndexWriter::startIndexable(Indexable* idx) {
    // get the file name
    const char* name = idx->getName().c_str();
    size_t namelen = idx->getName().length();

    // remove old entry
    vector<string> v;
    v.push_back(idx->getName());
    deleteEntries(v);

    sqlite3* db = manager->ref();
    assert(db == dbcheck);
    int64_t id = -1;
    sqlite3_bind_text(insertfilestmt, 1, name, namelen, SQLITE_STATIC);
    sqlite3_bind_int64(insertfilestmt, 2, idx->getMTime());
    sqlite3_bind_int(insertfilestmt, 3, idx->getDepth());
//    printf("'%s', %i, %i %p %p\n", name, idx->getMTime(), idx->getDepth(), db, insertfilestmt);
    int r = sqlite3_step(insertfilestmt);
    if (r != SQLITE_DONE) {
        if (r == SQLITE_ERROR) printf("error!\n");
        printf("error in adding file %i %s\n", r, sqlite3_errmsg(db));
        exit(1);
    }
    id = sqlite3_last_insert_rowid(db);
    sqlite3_reset(insertfilestmt);
    idx->setId(id);
    manager->deref();
}
/*
    Close all left open indexwriters for this path.
*/
void
SqliteIndexWriter::finishIndexable(const Indexable* idx) {
    // store the content field
    map<int64_t, map<string, int> >::const_iterator m
        = content.find(idx->getId());

    if (m == content.end()) {
        return;
    }

    sqlite3* db = manager->ref();
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db,
        "insert into tempfilewords (fileid, word, count) values(?, ?,?);",
        0, &stmt, 0);
    if (r != SQLITE_OK) {
        if (idx->getDepth() == 0) {
            sqlite3_exec(db, "rollback; ", 0, 0, 0);
        }
        printf("could not prepare temp insert sql %s\n", sqlite3_errmsg(db));
        content.erase(m->first);
        manager->deref();
        return;
    }
    map<string, int>::const_iterator i = m->second.begin();
    map<string, int>::const_iterator e = m->second.end();
    sqlite3_bind_int64(stmt, 1, idx->getId());
    for (; i!=e; ++i) {
        sqlite3_bind_text(stmt, 2, i->first.c_str(), i->first.length(),
            SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, i->second);
        r = sqlite3_step(stmt);
        if (r != 21) { // what is 21?
            printf("could not write content into database: %i %s\n", r,
                sqlite3_errmsg(db));
        }
        r = sqlite3_reset(stmt);
        temprows++;
    }
    sqlite3_finalize(stmt);
    manager->deref();
    content.erase(m->first);
}
void
SqliteIndexWriter::commit() {
    printf("start commit\n");
    // move the data from the temp tables into the index

    const char* sql = "replace into words (wordid, word, count) "
        "select wordid, t.word, sum(t.count)+ifnull(words.count,0) "
        "from tempfilewords t left join words on t.word = words.word "
        "group by t.word; "
        "insert into filewords (fileid, wordid, count) "
        "select fileid, words.wordid, t.count from tempfilewords t join words "
        "on t.word = words.word;"
        "replace into idx select * from tempidx;"
        "delete from tempidx;"
        "delete from tempfilewords;"
        //"commit;"
        //"begin immediate transaction;"
        ;
    sqlite3* db = manager->ref();
    int r = sqlite3_exec(db, sql, 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not store new data: %s\n", sqlite3_errmsg(db));
    }
    manager->deref();
    printf("end commit of %i rows\n", temprows);
    temprows = 0;
}
/**
 * Delete all files that start with the specified path.
 **/
void
SqliteIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
    sqlite3* db = manager->ref();
    // turn on case sensitivity
    sqlite3_exec(db, "PRAGMA case_sensitive_like = 1", 0, 0, 0);

    sqlite3_stmt* delstmt;
    const char* delsql = "delete from files where path like ?;";
    prepareStmt(db, delstmt, delsql, strlen(delsql));
    vector<string>::const_iterator i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        string f = *i+'%';
        sqlite3_bind_text(delstmt, 1, f.c_str(), f.length(), SQLITE_STATIC);
        int r = sqlite3_step(delstmt);
        if (r != SQLITE_DONE) {
            printf("could not delete file %s: %s\n", i->c_str(),
                sqlite3_errmsg(db));
        }
        printf("removed entry '%s'\n", f.c_str());
        sqlite3_reset(delstmt);
    }
    sqlite3_finalize(delstmt);
    // turn off case sensitivity
    sqlite3_exec(db, "PRAGMA case_sensitive_like = 0", 0, 0, 0);

    // remove all information that now is orphaned
    int r = sqlite3_exec(db,
        "delete from idx where fileid not in (select fileid from files);"
        "delete from filewords where fileid not in (select fileid from files);"
        "update words set count = (select sum(f.count) from filewords f "
            "where words.wordid = f.wordid group by f.wordid);"
        "delete from words where count is null or count = 0;"
        , 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not delete associated information: %s\n",
            sqlite3_errmsg(db));
    }
    manager->deref();
}
void
SqliteIndexWriter::deleteAllEntries() {
    manager->ref();
    manager->deref();
}
