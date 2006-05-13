#include "digestthroughanalyzer.h"
#include "digestinputstream.h"
#include "inputstream.h"
#include <openssl/sha.h>
using namespace jstreams;

DigestThroughAnalyzer::DigestThroughAnalyzer() {
    stream = 0;
}
DigestThroughAnalyzer::~DigestThroughAnalyzer() {
    if (stream) {
        delete stream;
    }
}
InputStream *
DigestThroughAnalyzer::connectInputStream(InputStream *in) {
    if (stream) {
        delete stream;
    }
    stream = new DigestInputStream(in);
    return stream;
}
void
DigestThroughAnalyzer::setIndexable(jstreams::Indexable* idx) {
}
