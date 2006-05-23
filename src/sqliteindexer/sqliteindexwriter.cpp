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
        manager->deref();
        return;
    }
    // speed up by being unsafe and keeping temp tables in memory
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF;"
        "PRAGMA temp_store = MEMORY;", 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not speed up database\n");
    }
    // create the tables required
    const char* sql ="create table files (fileid integer primary key, "
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
        printf("could not create table %i %s\n", r, sqlite3_errmsg(db));
    }
    // prepare the insert statement
    sql = "insert or replace into idx (fileid, name, value) values(?, ?, ?)";
    r = sqlite3_prepare(db, sql, 0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare insert statement\n");
        stmt = 0;
    }
}
SqliteIndexWriter::~SqliteIndexWriter() {
    if (stmt) {
        int r = sqlite3_finalize(stmt);
        if (r != SQLITE_OK) {
            printf("could not finalize insert statement\n");
        }
    }
    if (db) {
        int r = sqlite3_close(db);
        if (r != SQLITE_OK) {
            printf("could not create table\n");
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
    sqlite3_bind_int64(stmt, 1, idx->getId());
    sqlite3_bind_text(stmt, 2, fieldname.c_str(),
        fieldname.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, value.c_str(), value.length(), SQLITE_STATIC);
    int r = sqlite3_step(stmt);
    if (r != SQLITE_DONE) {
        printf("could not write into database: %i %s\n", r, sqlite3_errmsg(db));
    }
    r = sqlite3_reset(stmt);
    if (r != SQLITE_OK) {
        printf("could not reset statement: %i %s\n", r, sqlite3_errmsg(db));
    }
    manager->deref();
}
void
SqliteIndexWriter::setField(const Indexable*, const std::string &fieldname,
        int64_t value) {
}

void
SqliteIndexWriter::startIndexable(Indexable* idx) {
    // get the file name
    string name = SqliteIndexManager::escapeSqlValue(idx->getName());

    // remove the previous version of this file
    // check if there is a previous version
    string sql = "select fileid, mtime from files where path = ?;";
    manager->ref();
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db,
        "select fileid, mtime from files where path = ?;", 0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare document find sql\n");
        idx->setId(-1);
        manager->deref();
        return;
    }
    r = sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
    r = sqlite3_step(stmt);
    if (r != SQLITE_ROW && r != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        printf("could not look for a document by path\n");
        manager->deref();
        return;
    }
    int64_t id = -1;
    int64_t mtime = -1;
    bool newfile;
    if (r == SQLITE_ROW) {
        id = sqlite3_column_int64(stmt, 0);
        mtime = sqlite3_column_int64(stmt, 1);
        sqlite3_finalize(stmt);
        if (mtime != idx->getMTime()) {
            setIndexed(idx, false);
            // TODO delete old data for this file
            r = sqlite3_prepare(db, "update files set mtime = ?;", 0, &stmt, 0);
            sqlite3_bind_int64(stmt, 1, idx->getMTime());
            r = sqlite3_step(stmt);
            if (r != SQLITE_DONE) {
                printf("error in adding file %i %s\n", r, sqlite3_errmsg(db));
            }
            sqlite3_finalize(stmt);
        }
    } else {
        setIndexed(idx, false);
        sqlite3_finalize(stmt);
        // prepare the insert statement
        r = sqlite3_prepare(db, "insert into files (path, mtime) "
            "values(?, ?);'", 0, &stmt, 0);
        if (r != SQLITE_OK) {
            printf("could not prepare document insert sql\n");
            idx->setId(-1);
            manager->deref();
            return;
        }
        sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, idx->getMTime());
        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            printf("error in adding file %i %s\n", r, sqlite3_errmsg(db));
        }
        id = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
    }
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
    if (m == content.end()) return;

    // create a temporary table
    manager->ref();
    int r = sqlite3_exec(db, "begin immediate transaction; "
        "create temp table t(word, count)", 0,0,0);
    if (r != SQLITE_OK) {
        printf("could not create temp table %i %s\n", r, sqlite3_errmsg(db));
    }

    sqlite3_stmt* stmt;
    r = sqlite3_prepare(db, "insert into t (word, count) values(?,?);",
         0, &stmt, 0);
    if (r != SQLITE_OK) {
        sqlite3_exec(db, "rollback; ", 0, 0, 0);
        printf("could not prepare temp insert sql\n");
        content.erase(m->first);
        manager->deref();
        return;
    }
    map<string, int>::const_iterator i = m->second.begin();
    map<string, int>::const_iterator e = m->second.end();
    for (; i!=e; ++i) {
        sqlite3_bind_text(stmt, 1, i->first.c_str(), i->first.length(),
            SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, i->second);
        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            printf("could not write into database: %i %s\n", r,
                sqlite3_errmsg(db));
        }
        r = sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);

    ostringstream sql;
    sql << "replace into words (wordid, word, count) "
        "select wordid, t.word, t.count+ifnull(words.count,0) "
        "from t left join words on t.word = words.word; "
        "replace into filewords (fileid, wordid, count) "
        "select ";
    sql << m->first;
    sql << ", words.wordid, t.count from t join words "
        "on t.word = words.word; drop table t; commit transaction;";
    r = sqlite3_exec(db, sql.str().c_str(),0,0,0);
    if (r != SQLITE_OK) {
        sqlite3_exec(db, "rollback; ", 0, 0, 0);
        printf("could not drop temp table %i %s\n", r, sqlite3_errmsg(db));
    }
    manager->deref();
    content.erase(m->first);
}
