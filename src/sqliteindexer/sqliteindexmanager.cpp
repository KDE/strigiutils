#include "sqliteindexmanager.h"
using namespace std;

pthread_mutex_t SqliteIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

SqliteIndexManager::SqliteIndexManager(const char* dbfile) {
    dblock = lock;
    this->dbfile = dbfile;
}
void
SqliteIndexManager::ref() {
    pthread_mutex_lock(&dblock);
}
void
SqliteIndexManager::deref() {
    pthread_mutex_unlock(&dblock);
}
