#ifndef CLUCENEINDEXWRITER_H
#define CLUCENEINDEXWRITER_H

#include "indexwriter.h"

class CLuceneIndexManager;
class CLuceneIndexWriter : public jstreams::IndexWriter {
private:
    CLuceneIndexManager* manager;
    int doccount;
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
    CLuceneIndexWriter(CLuceneIndexManager* m) :manager(m), doccount(0) {}
    ~CLuceneIndexWriter();
    void commit() {};
    void deleteEntries(const std::vector<std::string>& entries) {}
};

#endif
