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
#ifndef SQLITEPROPERTYWRITER_H
#define SQLITEPROPERTYWRITER_H

#include "searchstorage.h"

struct sqlite3;
struct sqlite3_stmt;

/**
 * Type of properties: String, Binary or Integer in combination with 1:1 or 1:n
 **/
class SqlitePropertyWriter : public PropertyWriter {
friend class SqliteStorage;
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;
public:
    SqlitePropertyWriter(sqlite3 *db, const std::string &tablename, PropertyType type, bool oneperfile);
    ~SqlitePropertyWriter();
    char writeProperty(long fileid, const char *value, int numBytes);
    char writeProperty(long fileid, const void *value, int numBytes);
    char writeProperty(long fileid, int value);
};

#endif
