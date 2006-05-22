#include "sqliteindexreader.h"
#include "sqliteindexmanager.h"
#include <sqlite3.h>
#include <set>
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
int
sqliteindexreadercallback(void*arg, int, char**v, char**) {
    std::vector<std::string>& results = *(std::vector<std::string>*)arg;
    results.push_back(v[1]);
    return 0;
}
set<string>
split(const std::string& q) {
    set<string> terms;
    uint pos = q.find(' ');
    uint offset = 0;
    while (pos != string::npos) {
        if (pos-offset > 0) {
            terms.insert(q.substr(offset, pos-offset));
        }
        offset = pos+1;
        pos = q.find(' ', offset);
    }
    if (offset < q.length()) {
        terms.insert(q.substr(offset));
    }
    return terms;
}
#include <sstream>
string
createQueryold(int n) {
    ostringstream q;
    q << "select path from files f ";
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        q <<"join filewords f"<<a<<" on f.rowid = f"<<a<<".fileid join words w"
          <<a<<" on f"<<a<<".wordid = w"<<a<<".wordid ";
    }
    q << "where ";
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        if (i > 0) q << "and ";
        q <<"w"<<a<<".word like ? ";
    }
    return q.str();
}
string
createQuery(int n) {
    // TODO: makeing a query with wildcards slows things down, so we must
    // think about reordering them
    // although we'll never be able to manage queries like 'a% b% c%'
    ostringstream q;
    q << "select path, ";
    // the points for a file is the sum of the fraction of the total occurances
    // of that word in this file
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        if (i > 0) q<<"+";
        q << "f"<<a<<".count*1.0/w"<<a<<".count";
    }
    q << " p from ";
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        q <<"words w"<<a<<",";
    }
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        q <<"filewords f"<<a<<",";
    }
    q <<"files where ";
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        q <<"w"<<a<<".word like ? and w"<<a<<".wordid = f"<<a<<".wordid and ";
    }
    for (int i=1; i<n; ++i) {
        char a = i+'a'-1;
        char b = i+'a';
        q <<"f"<<a<<".fileid = f"<<b<<".fileid and ";
    }
    q <<"fa.fileid = files.rowid order by p limit 100"; 
    return q.str();
}
vector<string>
SqliteIndexReader::query(const std::string& query) {
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
    // split up in terms
    set<string> terms = split(query);
    std::vector<std::string> results;
    if (terms.size() == 0) return results;

    // this sql probably warants some explanation
/*    string sql = "select sp, path from (select fileid, sum(p) sp from ("
        "select fileid, f.count*1.0/w.count p "
        "from words w join filewords f on w.wordid = f.wordid "
        "where word like '";
    sql += q;
    sql += "') group by fileid order by sp desc limit 100) "
        "join files on fileid = files.rowid;";
    char* error;
    int r = sqlite3_exec(db, sql.c_str(), sqliteindexreadercallback,
        &results, &error);
    if (r != SQLITE_OK && error) {
        printf("%s\n", error);
        sqlite3_free(error);
    }
*/
    string sql = createQuery(terms.size());
    manager->ref();
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db, sql.c_str(), 0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare query '%s': %s\n", sql.c_str(),
            sqlite3_errmsg(db));
        manager->deref();
        return results;
    }
    set<string>::const_iterator i;
    int j = 1;
    for (i=terms.begin(); i!=terms.end(); ++i) {
        sqlite3_bind_text(stmt, j++, i->c_str(), i->length(),
            SQLITE_STATIC);
    }
    r = sqlite3_step(stmt);
    while (r == SQLITE_ROW) {
        results.push_back((const char*)sqlite3_column_text(stmt, 0));
        r = sqlite3_step(stmt);
    }
    if (r != SQLITE_DONE) {
        printf("error reading query results: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    manager->deref();
    return results;
}
