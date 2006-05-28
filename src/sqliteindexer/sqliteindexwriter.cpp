#include "sqliteindexwriter.h"
#include "sqliteindexmanager.h"
#include <sqlite3.h>
#include <sstream>
using namespace std;
using namespace jstreams;

SqliteIndexWriter::SqliteIndexWriter(SqliteIndexManager *m)
        : manager(m) {

    int r = sqlite3_open(manager->getDBFile(), &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        printf("could not open db\n");
        db = 0;
        return;
    }

    // create temporary tables
    const char* sql = "PRAGMA synchronous = OFF;"
        "PRAGMA auto_vacuum = 1;"
        "PRAGMA temp_store = MEMORY;"
        "create temp table tempidx (fileid integer, name text, value);"
        "create temp table tempfilewords (fileid integer, word text, "
            "count integer); "
        "create index tempfilewords_word on tempfilewords(word);"
        "create index tempfilewords_fileid on tempfilewords(fileid);"
        // "begin immediate transaction;"
        ;
    r = sqlite3_exec(db, sql, 0,0,0);
    if (r != SQLITE_OK) {
        printf("could not init writer: %i %s\n", r, sqlite3_errmsg(db));
    }
    temprows = 0;

    // prepare the sql statements
    sql = "insert into tempidx (fileid, name, value) values(?, ?, ?)";
    prepareStmt(insertvaluestmt, sql, strlen(sql));
    sql = "select fileid, mtime from files where path = ?;";
    prepareStmt(getfilestmt, sql, strlen(sql));
    sql = "update files set mtime = ?;";
    prepareStmt(updatefilestmt, sql, strlen(sql));
    sql = "insert into files (path, mtime, depth) values(?, ?, ?);'";
    prepareStmt(insertfilestmt, sql, strlen(sql));
}
SqliteIndexWriter::~SqliteIndexWriter() {
    commit();
    finalizeStmt(insertvaluestmt);
    finalizeStmt(getfilestmt);
    finalizeStmt(updatefilestmt);
    finalizeStmt(insertfilestmt);
    if (db) {
        int r = sqlite3_close(db);
        if (r != SQLITE_OK) {
            printf("could not close the database\n");
        }
    }
}
void
SqliteIndexWriter::prepareStmt(sqlite3_stmt*& stmt, const char* sql,
        int sqllength) {
    int r = sqlite3_prepare(db, sql, sqllength,& stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare statement '%s': %s\n", sql,
            sqlite3_errmsg(db));
        stmt = 0;
    }
}
void
SqliteIndexWriter::finalizeStmt(sqlite3_stmt*& stmt) {
    if (stmt) {
        int r = sqlite3_finalize(stmt);
        stmt = 0;
        if (r != SQLITE_OK) {
            printf("could not prepare statement: %s\n", sqlite3_errmsg(db));
        }
    }
}
void
SqliteIndexWriter::addStream(const Indexable* idx, const string& fieldname,
        StreamBase<wchar_t>* datastream) {
}
void
SqliteIndexWriter::addField(const Indexable* idx, const string &fieldname,
        const string& value) {
    int64_t id = idx->getId();
    //id = -1; // debug
    if (id == -1) return;
    if (fieldname == "content") {
        // find a pointer to the value
        map<int64_t, map<string, int> >::iterator m = content.find(id);
        if (m == content.end()) {
            // no value at all yet, so fine to add one
            content[id][value]++;
        } else if (m->second.size() >= 1000) {
            map<string, int>::iterator i = m->second.find(value);
            if (i != m->second.end()) {
                i->second++;
            }
        } else {
            content[id][value]++;
        }
            
        return;
    }
    manager->ref();
    sqlite3_bind_int64(insertvaluestmt, 1, idx->getId());
    sqlite3_bind_text(insertvaluestmt, 2, fieldname.c_str(),
        fieldname.length(), SQLITE_STATIC);
    sqlite3_bind_text(insertvaluestmt, 3, value.c_str(), value.length(),
        SQLITE_STATIC);
    int r = sqlite3_step(insertvaluestmt);
    if (r != SQLITE_DONE) {
        printf("could not write into database: %i %s\n", r, sqlite3_errmsg(db));
    }
    r = sqlite3_reset(insertvaluestmt);
    if (r != SQLITE_OK) {
        printf("could not reset statement: %i %s\n", r, sqlite3_errmsg(db));
    }
    temprows++;
    manager->deref();
}
void
SqliteIndexWriter::setField(const Indexable*, const std::string &fieldname,
        int64_t value) {
}

void
SqliteIndexWriter::startIndexable(Indexable* idx) {
    // get the file name
    const char* name = idx->getName().c_str();
    size_t namelen = idx->getName().length();

    manager->ref();
    int64_t id = -1;
    sqlite3_bind_text(insertfilestmt, 1, name, namelen, SQLITE_STATIC);
    sqlite3_bind_int64(insertfilestmt, 2, idx->getMTime());
    sqlite3_bind_int(insertfilestmt, 3, idx->getDepth());
    int r = sqlite3_step(insertfilestmt);
    if (r != SQLITE_DONE) {
        printf("error in adding file %i %s\n", r, sqlite3_errmsg(db));
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

    manager->ref();
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
        if (r != SQLITE_DONE) {
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
    manager->ref();
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
SqliteIndexWriter::deleteEntry(const string& path) {
    int64_t id = -1;
    manager->ref();
    // turn on case sensitivity
    sqlite3_exec(db, "PRAGMA case_sensitive_like = 1", 0, 0, 0);

    sqlite3_stmt* getstmt;
    string pathplus = path+"%";
    const char* getsql = "select fileid from files where path like ?;";
    prepareStmt(getstmt, getsql, strlen(getsql));
    sqlite3_bind_text(getstmt, 1, pathplus.c_str(), pathplus.length(),
        SQLITE_STATIC);
    int r = sqlite3_step(getstmt);
    while (r == SQLITE_ROW) {
        id = sqlite3_column_int64(getstmt, 0);
        ostringstream sql;
        // todo adapt the word counts
        sql << "delete from idx where fileid = " << id
            << "; delete from filewords where fileid = " << id
            << "; delete from files where fileid = " << id;
        int sr = sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
        if (sr != SQLITE_OK) {
            printf("could not delete file %s: %s\n", path.c_str(),
                sqlite3_errmsg(db));
        }
        r = sqlite3_step(getstmt);
    }
    sqlite3_finalize(getstmt);
    if (r != SQLITE_DONE) {
        printf("error in deleting file %s:\n", path.c_str());
    }

    // turn off case sensitivity
    sqlite3_exec(db, "PRAGMA case_sensitive_like = 0", 0, 0, 0);
    manager->deref();
}
