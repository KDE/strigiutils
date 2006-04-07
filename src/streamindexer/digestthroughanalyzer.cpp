#include "digestthroughanalyzer.h"
#include "inputstream.h"
#include <openssl/sha.h>

class DigestInputStream : public InputStream {
private:
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    InputStream *input;
public:
    DigestInputStream(InputStream *input);
    Status read(const char*& start, size_t& read, size_t max = 0);
    Status skip(size_t ntoskip);
    Status mark(size_t readlimit);
    Status reset();
    void printDigest();
};
DigestInputStream::DigestInputStream(InputStream *input) {
    this->input = input;
    SHA1_Init(&sha1);
}
InputStream::Status
DigestInputStream::read(const char*& start, size_t& read, size_t max) {
    Status r = input->read(start, read, max);
    if (r) return r;
    SHA1_Update(&sha1, start, read);
    return r;
}
InputStream::Status
DigestInputStream::skip(size_t ntoskip) {
    // we cannot just skip but must read the data
    const char*start;
    size_t read;
    return input->read(start, read, ntoskip);
}
InputStream::Status
DigestInputStream::mark(size_t readlimit) {
    // TODO fix this stream for rereading data
    return input->mark(readlimit);
}
InputStream::Status
DigestInputStream::reset() {
    // TODO fix this stream for rereading data
    return input->reset();
}
void
DigestInputStream::printDigest() {
    SHA1_Final(digest, &sha1);
    printf("The hash: ");
    for (int i = 0; i < 20; i++) {
        printf("%02x ", digest[i]);
    }
    printf("\n");
}
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
DigestThroughAnalyzer::printResults() {
    if (stream) {
        stream->printDigest();
    }
}
