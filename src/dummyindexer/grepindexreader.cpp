#include "grepindexreader.h"
using namespace std;
using namespace Strigi;

GrepIndexReader::GrepIndexReader(const string& dir) {
}
int32_t
GrepIndexReader::countHits(const Query& query) {
    return -1;
}
vector<IndexedDocument>
GrepIndexReader::query(const Query&) {
    vector<IndexedDocument> hits;
    return hits;
}
map<string, time_t>
GrepIndexReader::files(char depth) {
    map<string, time_t> files;
    return files;
}
int32_t
GrepIndexReader::countDocuments() { return -1; }
int32_t
GrepIndexReader::countWords() { return -1; }
int64_t
GrepIndexReader::indexSize() { return -1; }
int64_t
GrepIndexReader::documentId(const string& uri) {
    return -1;
}
time_t
GrepIndexReader::mTime(int64_t docid) {
    return -1;
}
vector<string>
GrepIndexReader::fieldNames() {
    vector<string> fieldnames;
    return fieldnames;
}
vector<pair<string,uint32_t> >
GrepIndexReader::histogram(const string& query, const string& fieldname,
            const string& labeltype) {
    vector<pair<string,uint32_t> > histogram;
    return histogram;
}
int32_t
GrepIndexReader::countKeywords(const string& keywordprefix,
        const vector<string>& fieldnames) {
    return -1;
}
vector<string>
GrepIndexReader::keywords(
        const string& keywordmatch,
        const vector<string>& fieldnames,
        uint32_t max, uint32_t offset) {
    vector<string> keywords;
    return keywords;
}
int
main() {
    GrepIndexReader("/home");
    return 0;
}
