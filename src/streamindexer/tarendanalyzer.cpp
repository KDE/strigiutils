#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "streamindexer.h"
#include "subinputstream.h"
using namespace jstreams;

bool
TarEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return TarInputStream::checkHeader(header, headersize);
}
char
TarEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    return staticAnalyze(filename, in, depth, indexer, idx);
}
char
TarEndAnalyzer::staticAnalyze(std::string filename, jstreams::InputStream *in,
        int depth, jstreams::StreamIndexer *indexer, jstreams::Indexable*) {
    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
    while (s) {
        std::string file = filename+"/";
        file += tar.getEntryInfo().filename;
        indexer->analyze(file, tar.getEntryInfo().mtime, s, depth);
        s = tar.nextEntry();
    }
    if (tar.getStatus() == jstreams::Error) {
        return -1;
//        printf("Error: %s\n", tar.getError());
    } else {
//        printf("finished ok\n");
    }
    return 0;
}

