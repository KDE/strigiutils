#include "sqliteindexmanager.h"
#include "sqliteindexreader.h"
#include "sqliteindexwriter.h"
#include "sqlite3.h"
using namespace std;
using namespace jstreams;

pthread_mutex_t SqliteIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

jstreams::IndexManager*
createSqliteIndexManager(const char* path) {
    return new SqliteIndexManager(path);
}

SqliteIndexManager::SqliteIndexManager(const char* dbfile) {
    dblock = lock;
    this->dbfile = dbfile;

    sqlite3* db;
    int r = sqlite3_open(dbfile, &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        printf("could not open db\n");
        return;
    }
    // speed up by being unsafe and keeping temp tables in memory
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF;"
        "PRAGMA auto_vacuum = 1;"
        "PRAGMA temp_store = MEMORY;", 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not speed up database\n");
    }
    // create the tables required
    const char* sql ="create table files (fileid integer primary key, "
        "path text, mtime integer, size integer, depth integer,"
        "unique (path));"
        "create index files_mtime on files(mtime);"
        "create index files_size on files(size);"
        "create table idx (fileid integer, name text, value, "
        "unique (fileid, name, value) on conflict ignore);"
        "create index idx_fileid on idx(fileid);"
        "create index idx_name on idx(name);"
        "create index idx_value on idx(value);"
        "create table words (wordid integer primary key, "
        "    word, count, unique(word));"
        "create table filewords (fileid integer, wordid integer, count,"
        "unique (fileid, wordid));"
        "create index filewords_wordid on filewords(wordid);"
;
    r = sqlite3_exec(db, sql, 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not create table %i %s\n", r, sqlite3_errmsg(db));
    }
    sqlite3_close(db);
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
string
SqliteIndexManager::escapeSqlValue(const string& value) {
    string v = value;
    // replace ' by ''
    size_t p = v.find('\'');
    while (p != string::npos) {
        v.replace(p, 1, "''");
        p = v.find('\'', p+2);
    }
    return v; 
}    
