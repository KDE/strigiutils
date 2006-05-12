#ifndef DIGESTTHROUGHANALYZER_H
#define DIGESTTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
class DigestInputStream;
class DigestThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    DigestInputStream *stream;
public:
    DigestThroughAnalyzer();
    ~DigestThroughAnalyzer();
    void setIndexable(jstreams::Indexable*);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

#endif
