#include "bz2endanalyzer.h"
#include "bz2inputstream.h"
#include "streamindexer.h"

char
BZ2EndAnalyzer::analyze(std::string filename, InputStream *in, int depth, StreamIndexer *indexer) {
    BZ2InputStream stream(in);
    char r = testStream(&stream);
    if (r) {
        return r;
    }
    std::string file = filename+"/bunzipped";
    return indexer->analyze(file, &stream, depth);
}
