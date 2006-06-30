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
#include "sqlitepropertywriter.h"
#include <sqlite3.h>

SqlitePropertyWriter::SqlitePropertyWriter(sqlite3 *db,
        const std::string &tablename, PropertyType type, bool oneperfile) {
    this->db = db;
    this->type = type;
    printf("table: %s\n", tablename.c_str());
    std::string sql = "insert or replace into "+tablename+" values(?, ?)";
    int r = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
}
SqlitePropertyWriter::~SqlitePropertyWriter() {
    int r = sqlite3_finalize(stmt);
}
char
SqlitePropertyWriter::writeProperty(long fileid, const char *value, int numBytes) {
    if (type != PropertyWriter::STRING) return -2;
    int r;
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, 1, fileid);
    sqlite3_bind_text(stmt, 2, value, numBytes, SQLITE_STATIC);
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE)
    printf("write result %i %s\n", r, sqlite3_errmsg(db));
    return 0;
}
char
SqlitePropertyWriter::writeProperty(long fileid, const void *value, int numBytes) {
    if (type != PropertyWriter::BINARY) return -2;
    int r;
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, 1, fileid);
    sqlite3_bind_blob(stmt, 2, value, numBytes, SQLITE_STATIC);
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE)
    printf("write result %i %s\n", r, sqlite3_errmsg(db));
    return 0;
}
char
SqlitePropertyWriter::writeProperty(long fileid, int value) {
    if (type != PropertyWriter::INT) return -2;
    int r;
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, 1, fileid);
    sqlite3_bind_int(stmt, 2, value);
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE)
    printf("write result %i %s\n", r, sqlite3_errmsg(db));
    return 0;
}
