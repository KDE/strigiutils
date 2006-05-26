#ifndef CLUCENEINDEXREADER_H
#define CLUCENEINDEXREADER_H

#include "indexreader.h"
#include <map>

class CLuceneIndexManager;
namespace lucene {
    namespace search {
        class IndexSearcher;
    }
    namespace analysis {
        namespace standard {
            class StandardAnalyzer;
        }
    }
}

class CLuceneIndexReader : public jstreams::IndexReader {
friend class CLuceneIndexManager;
private:
    lucene::search::IndexSearcher* searcher;
    lucene::analysis::standard::StandardAnalyzer* analyzer;
    CLuceneIndexReader(const char* path);
    ~CLuceneIndexReader();
public:
    std::vector<std::string> query(const std::string&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
};

#endif
