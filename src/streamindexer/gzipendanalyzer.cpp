#include "gzipendanalyzer.h"
#include "gzipinputstream.h"
#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "tarendanalyzer.h"
#include "streamindexer.h"
#include "indexwriter.h"
using namespace jstreams;

bool
GZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return headersize > 2 && header[0] == 0x1f && header[1] == 0x8b;
}
char
GZipEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    GZipInputStream stream(in);
    // since this is gzip file, its likely that it contains a tar file
    const char* start;
    int32_t nread = stream.read(start, 1024, 0);
    if (nread < -1) {
        printf("Error reading gzip: %s\n", stream.getError());
        return -2;
    }
    stream.reset(0);
    if (TarInputStream::checkHeader(start, nread)) {
        return TarEndAnalyzer::staticAnalyze(filename, &stream, depth, indexer,
            idx);
    } else {
        std::string file = filename+"/bunzipped";
        return indexer->analyze(file, idx->getMTime(), &stream, depth);
    }
}
