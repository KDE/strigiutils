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
