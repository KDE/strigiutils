#ifndef STREAMTHROUGHANALYZER_H
#define STREAMTHROUGHANALYZER_H

#include <string>

class InputStream;
class StreamIndexer;

class StreamThroughAnalyzer {
public:
    virtual ~StreamThroughAnalyzer() {};
    virtual InputStream *connectInputStream(InputStream *in) = 0;
    virtual void printResults() = 0;
};

#endif
