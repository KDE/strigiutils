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
#include "sqlitestorage.h"
#include "sqlitepropertywriter.h"

#include <sstream>
#include <sqlite3.h>

SqliteStorage::SqliteStorage(const char *filename) {
    writer = 0;
    transaction = false;
    // open the database
    int r;
    r = sqlite3_open(filename, &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        status = -2;
        handleError(r);
    }
    startTransaction();
    // check main property table
    r = checkForTable("properties");
    if (r == 0) {
        createPropertyTable();
    }
}
SqliteStorage::~SqliteStorage() {
    if (writer) {
        delete writer;
    }
    if (transaction) {
        endTransaction();
    }
    int r = sqlite3_close(db);
    if (r) {
    }
}
void
SqliteStorage::defineProperty(const char *propertyname, PropertyWriter::PropertyType type, bool onetoone) {
    // check that the property was not yet defined
    // look for the property with the desired name
    PropertyWriter::PropertyType oldtype;
    std::string tablename;
    bool oldonetoone;
    char r = getPropertyTablename(propertyname, tablename, type, onetoone);
    if (tablename.length() == 0) {
        createPropertyTable(propertyname, tablename, type, onetoone);
    }
}
void
SqliteStorage::createPropertyTable(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType type, bool onetoone) {
    std::string sql = "insert into properties values(?, ?, ?)";
    sqlite3_stmt *stmt;
    int r;
    r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
    if (r != SQLITE_OK) {
        return;
    }
    sqlite3_bind_text(stmt, 1, propertyname, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, type);
    sqlite3_bind_int(stmt, 3, onetoone);
    r = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    long rowid = sqlite3_last_insert_rowid(db);
    std::ostringstream s;
    s << "t" << rowid;
    tablename = s.str();

    s.str("");
    s << "create table " << tablename << " (fileid integer";
    if (onetoone) {
        s << " primary key";
    }
    s << ", value ";
    switch (type) {
    case PropertyWriter::STRING:
        s << "text)";
        break;
    case PropertyWriter::INT:
        s << "integer)";
        break;
    case PropertyWriter::BINARY:
        s << "blob)";
    }
    sql = s.str();
    printf("'%s'\n", sql.c_str());
    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    s.str("");
    s << "create index " << tablename << "_value on " << tablename << "(value)";
    sql = s.str();
    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
}
PropertyWriter *
SqliteStorage::getPropertyWriter(const char *propertyname) {
    // look for the property with the desired name
    std::string tablename;
    PropertyWriter::PropertyType type;
    bool oneperfile;
    char r = getPropertyTablename(propertyname, tablename, type, oneperfile);
    if (tablename.length() == 0) {
        // this property was not defined
        return 0;
    }
    if (writer) {
        delete writer;
    }
    writer = new SqlitePropertyWriter(db, tablename, type, oneperfile);
    return writer;
}
char
SqliteStorage::getPropertyTablename(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType &type, bool &onetoone) {
    std::string sql = "select rowid, type, oneperfile from properties where propertyname = ?";
    sqlite3_stmt *stmt;
    int r;
    r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
    if (r != SQLITE_OK) return -2;
    r = sqlite3_bind_text(stmt, 1, propertyname, -1, SQLITE_STATIC);
    r = sqlite3_step(stmt);
    tablename.resize(0);
    std::ostringstream s;
    switch (r) {
    case SQLITE_DONE: // no data was retrieved
        printf("no result for %s\n", propertyname);
        break;
    case SQLITE_ROW:  // a new row is available
        s << "t" << sqlite3_column_int64(stmt, 0);
        tablename = s.str();
        type = (PropertyWriter::PropertyType)sqlite3_column_int(stmt, 1);
        onetoone = sqlite3_column_int(stmt, 2);
        break;
    case SQLITE_ERROR:
        printf("%s\n", sqlite3_errmsg(db));
    case SQLITE_MISUSE:
        break;
    case SQLITE_BUSY:
        break;
    }
    sqlite3_finalize(stmt);
}
/**
 * Check if a particular table exists.
 * Return value: 0: table does not exist, 1: table does exist -1: error occurred
 **/
char
SqliteStorage::checkForTable(const char *tablename) {
    printf("checkMainPropertyTable\n");
    std::string sql = "pragma table_info(";
    sql.append(tablename);
    sql.append(")");
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
    printf("%i\n", r);
    r = sqlite3_step(stmt);
    char rv;
    switch (r) {
    case SQLITE_DONE: // no data was retrieved
        rv = 0;
        break;
    case SQLITE_ROW:  // a new row is available
        rv = 1;
        break;
    case SQLITE_ERROR:
        printf("%s\n", sqlite3_errmsg(db));
    case SQLITE_MISUSE:
    case SQLITE_BUSY:
        rv = -1;
        break;
    }
    sqlite3_finalize(stmt);
    return rv;
}
char
SqliteStorage::createPropertyTable() {
    printf("createPropertyTable\n");
    std::string sql = "create table properties ( propertyname text unique not null, type integer not null, oneperfile integer not null)";
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
    r = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return r;
}
void
SqliteStorage::handleError(int r) {
    printf("%s\n", sqlite3_errmsg(db));
}
void
SqliteStorage::startTransaction() {
    if (transaction) {
        endTransaction();
    }
    transaction = true;
    sqlite3_exec(db, "begin immediate", 0, 0, 0);
}
void
SqliteStorage::endTransaction() {
    transaction = false;
    int r = sqlite3_exec(db, "end transaction", 0, 0, 0);
    timespec req;
    req.tv_sec = 0;
    while (r == SQLITE_BUSY) {
        // wait 0.01 second and try again
        req.tv_nsec = 10000;
        r = nanosleep(&req, 0);
        r = sqlite3_exec(db, "end transaction", 0, 0, 0);
    }
}
long
SqliteStorage::addFile(long parent, const char *name) {
    //printf("table: %s\n", tablename.c_str());
    std::string sql = "insert or replace into files values(?, ?)";
    //int r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
}
