#ifndef SQLITEINDEXMANAGER_H
#define SQLITEINDEXMANAGER_H

#include "indexmanager.h"
#include <sqlite3.h>
#include <pthread.h>
#include <string>
#include <map>

/**
 * Manage the database connection between threads.
 * http://www.sqlite.org/cvstrac/wiki?p=MultiThreading
 **/

struct sqlite3;
class SqliteIndexReader;
class SqliteIndexWriter;
class SqliteIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbfile;
    std::map<pthread_t, SqliteIndexReader*> readers;
    std::map<pthread_t, SqliteIndexWriter*> writers;
    std::map<pthread_t, sqlite3*> dbs;

public:
    SqliteIndexManager(const char* path);
    ~SqliteIndexManager();

    sqlite3* ref();
    void deref();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    static std::string escapeSqlValue(const std::string& value);
    static sqlite3* opendb(const char*);
};
jstreams::IndexManager*
createSqliteIndexManager(const char* path);

#endif
