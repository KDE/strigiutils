#ifndef TARENDANALYZER
#define TARENDANALYZER

#include "streamendanalyzer.h"

class TarEndAnalyzer : public StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        StreamIndexer *indexer);
};

#endif
