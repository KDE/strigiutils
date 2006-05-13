#include "zipendanalyzer.h"
#include "zipinputstream.h"
#include "streamindexer.h"
#include "subinputstream.h"
using namespace jstreams;

bool
ZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return ZipInputStream::checkHeader(header, headersize);
}
char
ZipEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable*) {
    const char* z;
    ZipInputStream zip(in);
    InputStream *s = zip.nextEntry();
    if (zip.getStatus()) {
        printf("error: %s\n", zip.getError());
        exit(1);
    }
    while (s) {
        std::string file = filename+"/";
        file += zip.getEntryInfo().filename;
        indexer->analyze(file, s, depth);
        s = zip.nextEntry();
    }
    if (zip.getStatus() == jstreams::Error) {
//        printf("Error: %s\n", tar.getError());
    } else {
//        printf("finished ok\n");
    }
    return zip.getStatus();
}

