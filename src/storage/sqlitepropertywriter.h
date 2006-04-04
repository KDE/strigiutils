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
