#ifndef XAPIANINDEXREADER_H
#define XAPIANINDEXREADER_H

#include "indexreader.h"
#include <map>
#include <xapian.h>

class XapianIndexManager;

class XapianIndexReader : public jstreams::IndexReader {
friend class XapianIndexManager;
private:
    XapianIndexManager* manager;
    Xapian::WritableDatabase* db;
    XapianIndexReader(XapianIndexManager* m, Xapian::WritableDatabase* d);
    ~XapianIndexReader();
public:
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
};

#endif
