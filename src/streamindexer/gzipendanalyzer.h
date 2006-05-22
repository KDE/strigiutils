#ifndef GZIPENDANALYZER
#define GZIPENDANALYZER

#include "streamendanalyzer.h"
#include "inputstream.h"

class GZipEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    const char* getName() const { return "GZipEndAnalyzer"; }
};

#endif
