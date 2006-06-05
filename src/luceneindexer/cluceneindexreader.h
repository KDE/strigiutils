#ifndef CLUCENEINDEXREADER_H
#define CLUCENEINDEXREADER_H

#include "indexreader.h"
#include <map>

namespace lucene {
    namespace index {
        class Term;
    }
    namespace search {
        class BooleanQuery;
    }
}

class CLuceneIndexManager;
class CLuceneIndexReader : public jstreams::IndexReader {
friend class CLuceneIndexManager;
private:
    CLuceneIndexManager* manager;
    CLuceneIndexReader(CLuceneIndexManager* m) :manager(m) {}
    ~CLuceneIndexReader();

    static const char* mapId(const std::string& id);
    static lucene::index::Term* createTerm(const std::string& name,
        const std::string& value);
    static void createBooleanQuery(const jstreams::Query& query,
        lucene::search::BooleanQuery& bq);
public:
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
    int countWords();
    int getIndexSize();
};

#endif
