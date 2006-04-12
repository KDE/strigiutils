#ifndef STREAMENDANALYZER_H
#define STREAMENDANALYZER_H

#include "inputstream.h"

class StreamIndexer;

class StreamEndAnalyzer {
protected:
    static char testStream(jstreams::InputStream *in);
public:
    virtual ~StreamEndAnalyzer() {};
    virtual char analyze(std::string filename, jstreams::InputStream *in, int depth, StreamIndexer *indexer) = 0;
};

#endif
