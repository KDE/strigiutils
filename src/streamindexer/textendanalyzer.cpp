#include "textendanalyzer.h"
#include "streamindexer.h"
#include "inputstreamreader.h"
#include "indexwriter.h"
using namespace jstreams;

char
TextEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, Indexable* i) {
    InputStreamReader reader(in);
    i->addStream("content", &reader);
    return 0;
}
