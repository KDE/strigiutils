#include "digestthroughanalyzer.h"
#include "digestinputstream.h"
#include "inputstream.h"
#include "indexwriter.h"
#include <openssl/sha.h>
using namespace jstreams;

DigestThroughAnalyzer::DigestThroughAnalyzer() {
    stream = 0;
    indexable = 0;
}
DigestThroughAnalyzer::~DigestThroughAnalyzer() {
    if (stream) {
        delete stream;
    }
}
InputStream *
DigestThroughAnalyzer::connectInputStream(InputStream *in) {
    if (indexable && stream && stream->getStatus() == Eof) {
        indexable->addField(L"sha1", stream->getDigestString().c_str());
    }
    if (stream) {
        delete stream;
    }
    stream = new DigestInputStream(in);
    return stream;
}
void
DigestThroughAnalyzer::setIndexable(jstreams::Indexable* idx) {
    indexable = idx;
}
