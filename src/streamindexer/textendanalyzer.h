#ifndef TEXTENDANALYZER
#define TEXTENDANALYZER

#include "streamendanalyzer.h"

class TextEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const {
        return true;
    }
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
};

#endif
