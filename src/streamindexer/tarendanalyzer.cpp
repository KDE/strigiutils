#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "streamindexer.h"
#include "subinputstream.h"
using namespace jstreams;

char
TarEndAnalyzer::analyze(std::string filename, InputStream *in, int depth, StreamIndexer *indexer) {
    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
    while (s) {
        std::string file = filename+"/";
        file += tar.getEntryInfo().filename;
        //printf("%s\n", file.c_str());
        //testStream(s);
        indexer->analyze(file, s, depth);
        s = tar.nextEntry();
    }
    if (tar.getError().size()) {
        //printf("%s\n", tar.getError().c_str());
    }
    return tar.getError().size() > 0;
}

