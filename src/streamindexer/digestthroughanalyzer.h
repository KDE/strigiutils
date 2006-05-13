#ifndef DIGESTTHROUGHANALYZER_H
#define DIGESTTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"

namespace jstreams {
	class DigestInputStream;
}

class DigestThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    jstreams::DigestInputStream *stream;
public:
    DigestThroughAnalyzer();
    ~DigestThroughAnalyzer();
    void setIndexable(jstreams::Indexable*);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

#endif
