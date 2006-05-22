#ifndef TARENDANALYZER
#define TARENDANALYZER

#include "streamendanalyzer.h"

class TarEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    static char staticAnalyze(std::string filename, jstreams::InputStream *in,
        int depth, jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    const char* getName() const { return "TarEndAnalyzer"; }
};

#endif
