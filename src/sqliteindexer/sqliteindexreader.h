#ifndef SQLITEINDEXREADER_H
#define SQLITEINDEXREADER_H

#include "indexreader.h"
#include <map>

struct sqlite3;
class SqliteIndexManager;

class SqliteIndexReader : public jstreams::IndexReader {
friend class SqliteIndexManager;
private:
    sqlite3 *db;
    SqliteIndexManager* manager;
    SqliteIndexReader(SqliteIndexManager* m);
    ~SqliteIndexReader();
public:
    std::vector<std::string> query(const std::string&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
};

#endif
