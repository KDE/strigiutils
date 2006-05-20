#include "sqliteindexreader.h"
#include "sqliteindexmanager.h"
#include <sqlite3.h>
using namespace std;

SqliteIndexReader::SqliteIndexReader(SqliteIndexManager* m) :manager(m) {
    manager->ref();

    int r = sqlite3_open(manager->getDBFile(), &db);
    // any value other than SQLITE_OK is an error
    if (r != SQLITE_OK) {
        printf("could not open db\n");
        db = 0;
        manager->deref();
        return;
    }
    // speed up by being unsafe (we're only reading anyway)
    r = sqlite3_exec(db, "PRAGMA synchronous = OFF", 0, 0, 0);
    if (r != SQLITE_OK) {
        printf("could not speed up database\n");
    }
    manager->deref();
}
SqliteIndexReader::~SqliteIndexReader() {
    manager->ref();
    if (db) {
        int r = sqlite3_close(db);
        if (r != SQLITE_OK) {
            printf("could not create table\n");
        }
    }
    manager->deref();
}
int
sqliteindexreadercallback(void*arg, int, char**v, char**) {
    std::vector<std::string>& results = *(std::vector<std::string>*)arg;
    results.push_back(*v);
    return 0;
}
std::vector<std::string>
SqliteIndexReader::query(const std::string& query) {
    string sql = "select path from idx where value like '";
    sql += query;
    sql += "'";
    std::vector<std::string> results;

    manager->ref();
    char* error;
    int r = sqlite3_exec(db, sql.c_str(), sqliteindexreadercallback,
        &results, &error);
    if (r != SQLITE_OK) {
        printf("%s\n", error);
        sqlite3_free(error);
    }
    manager->deref();

    printf("querying\n");
    return results;
}
