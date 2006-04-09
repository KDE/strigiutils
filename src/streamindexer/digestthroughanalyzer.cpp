#include "digestthroughanalyzer.h"
#include "inputstream.h"
#include <openssl/sha.h>

class DigestInputStream : public InputStream {
private:
    int32_t sinceMark;
    int32_t ignoreBytes;
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    InputStream *input;
public:
    DigestInputStream(InputStream *input);
    Status read(const char*& start, int32_t& read, int32_t max = 0);
    Status skip(int64_t ntoskip, int64_t* skipped = 0);
    Status mark(int32_t readlimit);
    Status reset();
    void printDigest();
};
DigestInputStream::DigestInputStream(InputStream *input) {
    this->input = input;
    status = Ok;
    sinceMark = 0;
    ignoreBytes = 0;
    SHA1_Init(&sha1);
}
InputStream::Status
DigestInputStream::read(const char*& start, int32_t& read, int32_t max) {
    if (status) return status;
    status = input->read(start, read, max);
    if (status == Error) {
        error = input->getError();
        return status;
    }
    if (status == Eof) {
        return Eof;
    }
    sinceMark += read;
    if (ignoreBytes < read) {
        SHA1_Update(&sha1, start+ignoreBytes, read-ignoreBytes);
        ignoreBytes = 0;
    } else {
        ignoreBytes -= read;
    }
    return status;
}
InputStream::Status
DigestInputStream::skip(int64_t ntoskip, int64_t* skipped) {
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    return InputStream::skip(ntoskip, skipped);
}
InputStream::Status
DigestInputStream::mark(int32_t readlimit) {
    if (status) return status;
    sinceMark = 0;
    return input->mark(readlimit);
}
InputStream::Status
DigestInputStream::reset() {
    Status s = input->reset();
    if (s == Ok) {
        ignoreBytes += sinceMark;
        sinceMark = 0;
    }
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
