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
#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include "searchstorage.h"

class SqliteStorage;
class SqlitePropertyWriter;

struct sqlite3;
struct sqlite3_stmt;

class SqliteStorage : public SearchStorage {
private:
    char status;
    bool transaction;
    sqlite3 *db;
    SqlitePropertyWriter *writer;

    void startTransaction();
    void endTransaction();
    void handleError(int r);
    char checkForTable(const char *);
    char createPropertyTable();
    void createPropertyTable(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType type, bool onetoone);
    char getPropertyTablename(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType &type, bool &onetoone);
public:
    SqliteStorage(const char *filename);
    ~SqliteStorage();
    void defineProperty(const char *propertyname, PropertyWriter::PropertyType type, bool onetoone);
    PropertyWriter *getPropertyWriter(const char *propertyname);
    long addFile(long parent, const char *name);
};

#endif
