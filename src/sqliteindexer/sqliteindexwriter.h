#ifndef SQLITEINDEXWRITER_H
#define SQLITEINDEXWRITER_H

#include "indexwriter.h"
#include <map>

struct sqlite3;
struct sqlite3_stmt;
class SqliteIndexManager;

class SqliteIndexWriter : public jstreams::IndexWriter {
friend class SqliteIndexManager;
private:
    sqlite3* dbcheck;
    SqliteIndexManager* manager;
    sqlite3_stmt *insertvaluestmt, *getfilestmt, *updatefilestmt,
        *insertfilestmt;
    const std::string indexpath;
    std::map<int64_t, std::map<std::string, int> > content;

    int temprows;
    void prepareStmt(sqlite3* db, sqlite3_stmt*& stmt, const char* sql,
        int sqllength);
    void finalizeStmt(sqlite3* db, sqlite3_stmt*& stmt);
protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length);
    void setField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
    SqliteIndexWriter(SqliteIndexManager*);
    ~SqliteIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    int itemsInCache() { return temprows; };
};

#endif
