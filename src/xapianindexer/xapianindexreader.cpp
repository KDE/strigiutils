#include "xapianindexreader.h"
#include "xapianindexmanager.h"
#include <xapian.h>
#include <set>
#include <sstream>
#include <assert.h>
using namespace std;
using namespace Xapian;

XapianIndexReader::XapianIndexReader(XapianIndexManager* m, WritableDatabase* d)
    : manager(m), db(d) {
}
XapianIndexReader::~XapianIndexReader() {
}
vector<string>
XapianIndexReader::query(const std::string& query) {
    std::vector<std::string> results;
    return results;
}
map<string, time_t>
XapianIndexReader::getFiles(char depth) {
    map<string, time_t> files;
    return files;
}
int
XapianIndexReader::countDocuments() {
    int count;
    manager->ref();
    manager->deref();
    return count;
}
