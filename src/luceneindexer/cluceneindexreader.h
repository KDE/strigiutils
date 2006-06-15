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
    namespace document {
        class Document;
        class Field;
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
    static std::string convertValue(const wchar_t* value);
    static void addField(lucene::document::Field* field,
        jstreams::IndexedDocument&);
public:
    int32_t countHits(const jstreams::Query&);
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int32_t countDocuments();
    int32_t countWords();
    int64_t getIndexSize();
};

#endif
