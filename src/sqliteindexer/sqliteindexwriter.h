#ifndef SQLITEINDEXWRITER_H
#define SQLITEINDEXWRITER_H

#include "indexwriter.h"
#include <vector>
#include <map>

struct sqlite3;
struct sqlite3_stmt;

class SqliteIndexWriter : public jstreams::IndexWriter {
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const std::string indexpath;
protected:
    void startIndexable(const jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addStream(const jstreams::Indexable*, const std::string& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
public:
    SqliteIndexWriter(const char* path);
    ~SqliteIndexWriter();
};

#endif
