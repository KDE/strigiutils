#ifndef XAPIANINDEXWRITER_H
#define XAPIANINDEXWRITER_H

#include "indexwriter.h"
#include <xapian.h>

class XapianIndexManager;
class XapianIndexWriter : public jstreams::IndexWriter {
friend class XapianIndexManager;
private:
    XapianIndexManager* manager;
    const std::string indexpath;
    Xapian::WritableDatabase* db;

protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addStream(const jstreams::Indexable*, const std::string& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
    void setField(const jstreams::Indexable*, const std::string &fieldname,
        int64_t value);
    XapianIndexWriter(XapianIndexManager*, Xapian::WritableDatabase*);
    ~XapianIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteEntry(const std::string& entry);
    int itemsInCache() { return 0; };
};

#endif
