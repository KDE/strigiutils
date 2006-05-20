#ifndef SQLITEINDEXMANAGER_H
#define SQLITEINDEXMANAGER_H

#include "indexmanager.h"
#include <pthread.h>
#include <string>

/**
 * Manage the database connection between threads.
 * http://www.sqlite.org/cvstrac/wiki?p=MultiThreading
 **/

struct sqlite3;
class SqliteIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbfile;
public:
    SqliteIndexManager(const char* path);
    const char* getDBFile() const { return dbfile.c_str(); }
   
    void ref();
    void deref();
};

#endif
