#ifndef CLUCENEINDEXWRITER_H
#define CLUCENEINDEXWRITER_H

#include "indexwriter.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include <vector>
#include <map>

class CLuceneIndexWriter : public jstreams::IndexWriter {
private:
    int doccount;
    std::string indexpath;
    lucene::index::IndexWriter* writer;
    lucene::analysis::Analyzer* analyzer;
    std::map<const jstreams::Indexable*, lucene::document::Document> docs;
    std::map<const jstreams::Indexable*, std::string> content;

protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addStream(const jstreams::Indexable*, const std::string& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable*, const std::string &fieldname,
        const std::string &value);
    void setField(const jstreams::Indexable*, const std::string &fieldname,
        int64_t value);
public:
    CLuceneIndexWriter(const char* path);
    ~CLuceneIndexWriter();
};

#endif
