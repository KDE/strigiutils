#ifndef BZ2ENDANALYZER
#define BZ2ENDANALYZER

#include "streamendanalyzer.h"
#include "inputstream.h"

class BZ2EndAnalyzer : public StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth, StreamIndexer *indexer);
};

#endif
