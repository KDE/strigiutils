#ifndef SQLITEINDEXREADER_H
#define SQLITEINDEXREADER_H

#include "indexreader.h"

struct sqlite3;
class SqliteIndexManager;

class SqliteIndexReader : public jstreams::IndexReader {
private:
    sqlite3 *db;
    SqliteIndexManager* manager;
public:
    SqliteIndexReader(SqliteIndexManager* m);
    ~SqliteIndexReader();
    std::vector<std::string> query(const std::string&);
};

#endif
