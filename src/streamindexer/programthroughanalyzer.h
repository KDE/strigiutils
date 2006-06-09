#ifndef PROGRAMTHROUGHANALYZER_H
#define PROGRAMTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"

class ProgramThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    jstreams::DigestInputStream *stream;
    jstreams::Indexable* indexable;
public:
    ProgramThroughAnalyzer();
    ~ProgramThroughAnalyzer();
    void setIndexable(jstreams::Indexable*);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

#endif
