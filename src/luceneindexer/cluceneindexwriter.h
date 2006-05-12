#ifndef CLUCENEINDEXWRITER_H
#define CLUCENEINDEXWRITER_H

#include "indexwriter.h"
#include <vector>
#include <map>

namespace lucene {
    namespace index {
        class IndexWriter;
    }
    namespace document {
        class Document;
    }
    namespace analysis {
        class Analyzer;
    }
}

class CLuceneIndexWriter : public jstreams::IndexWriter {
private:
    int activewriter;
    std::string indexespath;
    std::vector<lucene::index::IndexWriter*> writers;
    std::vector<lucene::analysis::Analyzer*> analyzers;
    std::map<const jstreams::Indexable*, lucene::document::Document*> docs;

    void addIndexWriter();
protected:
    void addStream(const jstreams::Indexable*, const std::wstring& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable*, const std::wstring &fieldname,
        const char* value);
    void addField(const jstreams::Indexable*, const std::wstring &fieldname,
        const std::wstring &value);
    void finishIndexable(const jstreams::Indexable*);
public:
    CLuceneIndexWriter(const char* path);
    ~CLuceneIndexWriter();
};

#endif
