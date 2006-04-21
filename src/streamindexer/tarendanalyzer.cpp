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
TarEndAnalyzer::analyze(std::string filename, InputStream *in, int depth, StreamIndexer *indexer) {
    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
    while (s) {
        std::string file = filename+"/";
        file += tar.getEntryInfo().filename;
        indexer->analyze(file, s, depth);
        s = tar.nextEntry();
    }
    if (tar.getStatus() == jstreams::Error) {
   //     printf("%s\n", tar.getError().c_str());
    }
    return tar.getStatus();
}

