#ifndef SAXENDANALYZER
#define SAXENDANALYZER

#include "streamendanalyzer.h"

class SaxEndAnalyzer : public jstreams::StreamEndAnalyzer {
private:
    class Private;
    Private* p;
public:
    SaxEndAnalyzer();
    ~SaxEndAnalyzer();
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    const char* getName() const { return "SaxEndAnalyzer"; }
};

#endif
