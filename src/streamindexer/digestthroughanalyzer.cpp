#include "digestthroughanalyzer.h"
#include "digestinputstream.h"
#include "inputstream.h"
#include "indexwriter.h"
#include <openssl/sha.h>
using namespace std;
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
    const static string sha1("sha1");
    if (indexable && stream) { // && stream->getStatus() == Eof) {
        indexable->addField(sha1, stream->getDigestString());
        printf("%s %s\n", indexable->getName().c_str(), stream->getDigestString().c_str());
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
