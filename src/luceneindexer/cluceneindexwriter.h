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
    void addText(const jstreams::Indexable*, const char* text, int32_t length);
    void setField(const jstreams::Indexable*, const std::string &fieldname,
        const std::string &value);
public:
    CLuceneIndexWriter(CLuceneIndexManager* m) :manager(m), doccount(0) {}
    ~CLuceneIndexWriter();
    void commit() {};
    void deleteEntries(const std::vector<std::string>& entries) {}
};

#endif
