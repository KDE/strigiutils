#ifndef DIGESTTHROUGHANALYZER_H
#define DIGESTTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
class DigestInputStream;
class DigestThroughAnalyzer : public StreamThroughAnalyzer {
private:
    DigestInputStream *stream;
    std::multimap<std::wstring, std::wstring> results;
public:
    DigestThroughAnalyzer();
    ~DigestThroughAnalyzer();
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
    const std::multimap<std::wstring, std::wstring> &getResults();
};

#endif
