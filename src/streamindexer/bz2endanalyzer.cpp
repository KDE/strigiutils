#include "bz2endanalyzer.h"
#include "bz2inputstream.h"
#include "streamindexer.h"
using namespace jstreams;

bool
BZ2EndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return BZ2InputStream::checkHeader(header, headersize);
}
char
BZ2EndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable*) {
    BZ2InputStream stream(in);
    char r = testStream(&stream);
    if (r) {
        return r;
    }
    std::string file = filename+"/bunzipped";
    return indexer->analyze(file, &stream, depth);
}
