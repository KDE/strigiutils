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
    // very simple algorithm to get out sequences of ascii characters
    // we actually miss characters that are not on the edge between reads
    const char* b;
    int32_t nread = in->read(b, 1, 0);
    // check that this is text file
    const char* end;
    const char* p;

    while (nread > 0) {
        end = b + nread;
        p = b;
        while (p != end) {
            // find the start of a word
            while (p < end && !isalpha(*p)) ++p;
            if (p != end) {
                const char* e = p + 1;
                while (e < end && isalpha(*e)) ++e;
                if (e != end && e-p > 2 && e-p < 30) {
                    std::string field(p, e-p);
//                    printf("%s %s\n", filename.c_str(), field.c_str());
                    i->addField("content", field);
                }
                p = e;
            }
        }
        nread = in->read(b, 1, 0);
    }
    if (nread != Eof) {
        error = in->getError();
        -1;
    }
//    InputStreamReader reader(in);
//    i->addStream("content", &reader);
    return 0;
}
