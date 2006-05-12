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
        int depth, StreamIndexer *indexer, jstreams::Indexable*) {
    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
//    printf("TAR %p %lli %i\n", s, in->getSize(), in->getStatus());
    while (s) {
        std::string file = filename+"/";
        file += tar.getEntryInfo().filename;
//        printf("entry %i %lli %s\n", depth, s->getSize(), file.c_str());
        indexer->analyze(file, s, depth);
        s = tar.nextEntry();
    }
    if (tar.getStatus() == jstreams::Error) {
//        printf("Error: %s\n", tar.getError());
    } else {
//        printf("finished ok\n");
    }
    return tar.getStatus();
}

