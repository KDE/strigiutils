#include "bz2endanalyzer.h"
#include "bz2inputstream.h"
#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "tarendanalyzer.h"
#include "streamindexer.h"
using namespace std;
using namespace jstreams;

bool
BZ2EndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    bool v = BZ2InputStream::checkHeader(header, headersize);
    return v;
}
char
BZ2EndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    BZ2InputStream stream(in);
/*    char r = testStream(&stream);
    if (r) {
        return r;
    }*/
    // since this is bz2 file, its likely that it contains a tar file
    const char* start;
    int32_t nread = stream.read(start, 1024, 0);
    if (nread < -1) {
        printf("Error reading bz2: %s\n", stream.getError());
        return -2;
    }
    stream.reset(0);
    if (TarInputStream::checkHeader(start, nread)) {
        return TarEndAnalyzer::staticAnalyze(filename, &stream, depth, indexer,
            idx);
    } else {
        std::string file;
        int p1 = filename.rfind("/");
        int len = filename.length();
        if (p1 != string::npos) {
            int len = filename.length();
            if (len > 4 && filename.substr(len-4)==".bz2") {
                file = filename + filename.substr(p1,len-p1-4);
            } else {
                file = filename + filename.substr(p1);
            }
        } else {
            // last resort
            file = filename+"/bunzipped";
        }
        return indexer->analyze(file, &stream, depth);
    }
}
