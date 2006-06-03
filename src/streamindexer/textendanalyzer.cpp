#include "textendanalyzer.h"
#include "streamindexer.h"
#include "inputstreamreader.h"
#include "indexwriter.h"
using namespace jstreams;

bool
TextEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    const char* end = header + headersize;
    const char* p = header-1;
    while (++p < end) {
        if (*p <= 8) {
            return false;
        }
    }
    return true;
}

char
TextEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, Indexable* i) {
    // pass a piece of text to the indexer. it's up to the indexer to break
    // it down into words
    const char* b;
    int32_t nread = in->read(b, 1024*1024, 0);
    if (nread > 0) {
        i->addText(b, nread);
    }
    if (nread != Eof) {
        error = in->getError();
        // TODO investigate
        //return -1;
    }
//    InputStreamReader reader(in);
//    i->addStream("content", &reader);
    return 0;
}
