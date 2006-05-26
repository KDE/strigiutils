#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::document::Document;
using lucene::index::Term;
using lucene::search::TermQuery;

CLuceneIndexReader::CLuceneIndexReader(const char* path) {
    analyzer = new StandardAnalyzer();
    searcher = new IndexSearcher(path);
}
CLuceneIndexReader::~CLuceneIndexReader() {
    delete searcher;
    delete analyzer;
}
std::vector<std::string>
CLuceneIndexReader::query(const std::string& query) {
    std::vector<std::string> results;
    printf("so you want info about %s\n", query.c_str());
    TCHAR tf[CL_MAX_DIR];
    char path[CL_MAX_DIR];
    STRCPY_AtoT(tf, query.c_str(), CL_MAX_DIR);
    Term term(_T("path"), tf);
    TermQuery termquery(&term);
    Hits *hits = searcher->search(&termquery);
    int s = hits->length();
    STRCPY_AtoT(tf, "path", CL_MAX_DIR);
    for (int i = 0; i < s; ++i) {
        Document *d = &hits->doc(i);
        const wchar_t *v = d->get(tf);
        STRCPY_TtoA(path, v, CL_MAX_DIR);
        results.push_back(path);
    }

    return results;
}
std::map<std::string, time_t>
CLuceneIndexReader::getFiles(char depth) {
    std::map<std::string, time_t> files;
    return files;
}
int
CLuceneIndexReader::countDocuments() {
    return 0;
}
