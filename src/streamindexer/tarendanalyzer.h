#ifndef TARENDANALYZER
#define TARENDANALYZER

#include "streamendanalyzer.h"

class TarEndAnalyzer : public StreamEndAnalyzer {
public:
    char analyze(std::string filename, InputStream *in, int depth, StreamIndexer *indexer);
};

#endif
