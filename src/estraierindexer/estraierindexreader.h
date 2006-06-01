#ifndef ESTRAIERINDEXREADER_H
#define ESTRAIERINDEXREADER_H

#include "indexreader.h"
#include <map>
#include <estraier.h>

class EstraierIndexManager;

class EstraierIndexReader : public jstreams::IndexReader {
friend class EstraierIndexManager;
private:
    EstraierIndexManager* manager;
    ESTDB* db;
    EstraierIndexReader(EstraierIndexManager* m, ESTDB* d);
    ~EstraierIndexReader();
public:
    std::vector<std::string> query(const std::string&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
};

#endif
