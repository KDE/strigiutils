#include "sqliteindexreader.h"
#include "sqliteindexmanager.h"
#include <set>
#include <sstream>
using namespace std;
using namespace jstreams;

SqliteIndexReader::SqliteIndexReader(SqliteIndexManager* m) :manager(m) {
}
SqliteIndexReader::~SqliteIndexReader() {
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
string
createQuery(int n, bool filterpath) {
    // TODO: makeing a query with wildcards slows things down, so we must
    // think about reordering them
    // although we'll never be able to manage queries like 'a% b% c%'
    ostringstream q;
    q << "select path";
    if (n > 0) q <<", sum(";
    // the points for a file is the sum of the fraction of the total occurances
    // of that word in this file
    for (int i=0; i<n; ++i) {
        char a = i+'a';
        if (i > 0) q<<"+";
        q << "f"<<a<<".count*1.0/w"<<a<<".count";
    }
    if (n > 0) {
    q <<") p ";
    }
    q <<" from ";
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
        char a = i+'a';
        q <<"fa.fileid = f"<<a<<".fileid and ";
    }
    if (n > 0) {
        q <<"fa.fileid = files.fileid ";
    }
    if (filterpath) {
        if (n > 0) q <<"and ";
        q <<"files.path like ? ";
    }
    if (n > 0) q <<"group by fa.fileid order by p ";
    q <<"limit 100"; 
    return q.str();
}
vector<IndexedDocument>
SqliteIndexReader::query(const std::string& query) {
    string q = query;
    // replace * by %
    size_t p = q.find('*');
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
    set<string> terms = split(q);
    std::vector<IndexedDocument> results;
    if (terms.size() == 0) return results;
    string pathfilter;
    set<string>::iterator i = terms.begin();
    while (i != terms.end()) {
        if (i->substr(0, 5) == "path:") {
            pathfilter = i->substr(5);
            terms.erase(i);
            i = terms.begin();
        } else {
            i++;
        }
    }
    if (terms.size() == 0 && pathfilter.length() == 0) return results;

    string sql = createQuery(terms.size(), pathfilter.length() > 0);
    //printf("%s\n", sql.c_str());

    sqlite3* db = manager->ref();
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db, sql.c_str(), 0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare query '%s': %s\n", sql.c_str(),
            sqlite3_errmsg(db));
        manager->deref();
        return results;
    }
    int j = 1;
    for (i=terms.begin(); i!=terms.end(); ++i) {
        sqlite3_bind_text(stmt, j++, i->c_str(), i->length(),
            SQLITE_STATIC);
    }
    if (pathfilter.length() > 0) {
        sqlite3_bind_text(stmt, j, pathfilter.c_str(), pathfilter.length(),
            SQLITE_STATIC);
    }
    r = sqlite3_step(stmt);
    while (r == SQLITE_ROW) {
        IndexedDocument doc;
        doc.filepath = (const char*)sqlite3_column_text(stmt, 0);
        results.push_back(doc);
        r = sqlite3_step(stmt);
    }
    if (r != SQLITE_DONE) {
        printf("error reading query results: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    manager->deref();
    return results;
}
map<string, time_t>
SqliteIndexReader::getFiles(char depth) {
    map<string, time_t> files;
    sqlite3* db = manager->ref();
    printf("%p\n", db);
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db, "select path, mtime from files where depth = ?",
        0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare query: %s\n", sqlite3_errmsg(db));
        manager->deref();
        return files;
    }
    sqlite3_bind_int(stmt, 1, depth);
    r = sqlite3_step(stmt);
    while (r == SQLITE_ROW) {
        const char *path = (const char*)sqlite3_column_text(stmt, 0);
        time_t mtime = sqlite3_column_int(stmt, 1);
        files[path] = mtime;
        r = sqlite3_step(stmt);
    }
    if (r != SQLITE_DONE) {
        printf("error reading query results: %i %s\n", r, sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    manager->deref();
    return files;
}
int
SqliteIndexReader::countDocuments() {
    sqlite3* db = manager->ref();
    sqlite3_stmt* stmt;
    int r = sqlite3_prepare(db, "select count(*) from files;", 0, &stmt, 0);
    if (r != SQLITE_OK) {
        printf("could not prepare count query: %s\n", sqlite3_errmsg(db));
        manager->deref();
        return -1;
    }
    r = sqlite3_step(stmt);
    if (r != SQLITE_ROW) {
        printf("could not read count query: %i %s\n", r, sqlite3_errmsg(db));
        manager->deref();
        return -1;
    }
    int count = sqlite3_column_int(stmt, 0);
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE) {
        printf("error reading count query results: %i %s\n", r,
            sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    manager->deref();
    return count;
}
