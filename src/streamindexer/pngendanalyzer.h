#ifndef PNGENDANALYZER
#define PNGENDANALYZER

#include "streamendanalyzer.h"

class PngEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
};

#endif
