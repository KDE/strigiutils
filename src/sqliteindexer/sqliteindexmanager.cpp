#include "sqliteindexmanager.h"
#include "sqliteindexreader.h"
#include "sqliteindexwriter.h"
using namespace std;
using namespace jstreams;

pthread_mutex_t SqliteIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

SqliteIndexManager::SqliteIndexManager(const char* dbfile) {
    dblock = lock;
    this->dbfile = dbfile;
}
SqliteIndexManager::~SqliteIndexManager() {
    std::map<pthread_t, SqliteIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, SqliteIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
}
void
SqliteIndexManager::ref() {
    pthread_mutex_lock(&dblock);
}
void
SqliteIndexManager::deref() {
    pthread_mutex_unlock(&dblock);
}
IndexReader*
SqliteIndexManager::getIndexReader() {
    pthread_t self = pthread_self();
    ref();
    SqliteIndexReader* r = readers[self];
    if (r == 0) {
        r = new SqliteIndexReader(this);
        readers[self] = r;
    }
    deref();
    return r;
}
IndexWriter*
SqliteIndexManager::getIndexWriter() {
    pthread_t self = pthread_self();
    ref();
    SqliteIndexWriter* w = writers[self];
    if (w == 0) {
        w = new SqliteIndexWriter(this);
        writers[self] = w;
    }
    deref();
    return w;
}
