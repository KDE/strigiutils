#include "sqliteindexwriter.h"
#include "sqliteindexmanager.h"
#include <sqlite3.h>
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
    // speed up by being unsafe
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF", 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not speed up database\n");
    }
    // create the tables required
    const char* sql ="create table files (path,"
        "unique (path) on conflict ignore);"
        "create table idx (path, name, value, "
        "unique (path, name, value) on conflict ignore);"
        "create index idx_path on idx(path);"
        "create index idx_name on idx(name);"
        "create index idx_value on idx(value);";
    r = sqlite3_exec(db, sql, 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not create table\n");
    }
    // prepare the insert statement
    sql = "insert into idx (path, name, value) values(?, ?, ?)";
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
    manager->ref();
    sqlite3_bind_text(stmt, 1, idx->getName().c_str(),
        idx->getName().length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, fieldname.c_str(),
        fieldname.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, value.c_str(), value.length(), SQLITE_STATIC);
    int r = sqlite3_step(stmt);
    if (r != SQLITE_DONE) {
        printf("could not write into database: %s\n", sqlite3_errmsg(db));
    }
    r = sqlite3_reset(stmt);
    if (r != SQLITE_OK) {
        printf("could not reset statement: %s\n", sqlite3_errmsg(db));
    }
    manager->deref();
}

void
SqliteIndexWriter::startIndexable(const Indexable* idx) {
}
/*
    Close all left open indexwriters for this path.
*/
void
SqliteIndexWriter::finishIndexable(const Indexable* idx) {
}
