#include "sqliteindexreader.h"
#include "sqliteindexmanager.h"
#include <sqlite3.h>
using namespace std;

SqliteIndexReader::SqliteIndexReader(SqliteIndexManager* m) :manager(m) {

    int r = sqlite3_open(manager->getDBFile(), &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        printf("could not open db\n");
        db = 0;
        return;
    }
    // speed up by being unsafe (we're only reading anyway)
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF", 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not speed up database\n");
    }
}
SqliteIndexReader::~SqliteIndexReader() {
    if (db) {
        int r = sqlite3_close(db);
        if (r != SQLITE_OK) {
            printf("could not create table\n");
        }
    }
}
int sqliteindexreadercallbackcount;
int
sqliteindexreadercallback(void*arg, int, char**v, char**) {
    if (sqliteindexreadercallbackcount++ == 1000) return 1;
    std::vector<std::string>& results = *(std::vector<std::string>*)arg;
    results.push_back(v[1]);
    return 0;
}
std::vector<std::string>
SqliteIndexReader::query(const std::string& query) {
    sqliteindexreadercallbackcount = 0;
    string q = query;
    // replace ' by ''
    size_t p = q.find('\'');
    while (p != string::npos) {
        q.replace(p, 1, "''");
        p = q.find('\'', p+2);
    }
    // replace * by %
    p = q.find('*');
    while (p != string::npos) {
        q.replace(p, 1, "%");
        p = q.find('*');
    }
    // replace ? by _
    p = q.find('?');
    while (p != string::npos) {
        q.replace(p, 1, "_");
        p = q.find('?');
    }
    // this sql probably warants some explanation
    string sql = "select sp, path from (select fileid, sum(p) sp from ("
        "select fileid, f.count*1.0/w.count p "
        "from words w join filewords f on w.wordid = f.wordid "
        "where word like '";
    sql += query;
    sql += "') group by fileid order by sp desc limit 100) "
        "join files on fileid = files.rowid;";
/*    string sql = "select distinct files.path from idx join files on idx.path = files.rowid where value like '";
    sql += q;
    sql += "'"; */
    std::vector<std::string> results;

    manager->ref();
    char* error;
    int r = sqlite3_exec(db, sql.c_str(), sqliteindexreadercallback,
        &results, &error);
    if (r != SQLITE_OK && error) {
        printf("%s\n", error);
        sqlite3_free(error);
    }
    manager->deref();

    printf("querying\n");
    return results;
}
