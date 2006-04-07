#ifndef DIGESTTHROUGHANALYZER_H
#define DIGESTTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
class DigestInputStream;
class DigestThroughAnalyzer : public StreamThroughAnalyzer {
private:
    DigestInputStream *stream;
public:
    DigestThroughAnalyzer();
    ~DigestThroughAnalyzer();
    InputStream *connectInputStream(InputStream *in);
    void printResults();
};

#endif
