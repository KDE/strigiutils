#ifndef STREAMENDANALYZER_H
#define STREAMENDANALYZER_H

#include <string>

class InputStream;
class StreamIndexer;

class StreamEndAnalyzer {
protected:
    static char testStream(InputStream *in);
public:
    virtual ~StreamEndAnalyzer() {};
    virtual char analyze(std::string filename, InputStream *in, int depth, StreamIndexer *indexer) = 0;
};

#endif
