#include "digestthroughanalyzer.h"
#include "inputstream.h"
#include <openssl/sha.h>
using namespace jstreams;

class DigestInputStream : public InputStream {
private:
    int32_t sinceMark;
    int32_t ignoreBytes;
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    InputStream *input;
public:
    DigestInputStream(InputStream *input);
    int32_t read(const char*& start);
    int32_t read(const char*& start, int32_t ntoread);
    StreamStatus skip(int64_t ntoskip, int64_t* skipped = 0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
    void printDigest();
};
DigestInputStream::DigestInputStream(InputStream *input) {
    this->input = input;
    status = Ok;
    sinceMark = 0;
    ignoreBytes = 0;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start) {
    int32_t nread = input->read(start);
    if (nread == -1) {
        error = input->getError();
        status = Error;
        return -1;
    }
    if (nread == 0) {
        status = Eof;
        return Eof;
    }
    sinceMark += nread;
    if (ignoreBytes < nread) {
        SHA1_Update(&sha1, start+ignoreBytes, nread-ignoreBytes);
        ignoreBytes = 0;
    } else {
        ignoreBytes -= nread;
    }
    return nread;
}
int32_t
DigestInputStream::read(const char*& start, int32_t ntoread) {
    int32_t nread = input->read(start, ntoread);
    if (nread == -1) {
        error = input->getError();
        status = Error;
        return -1;
    }
    if (nread != ntoread) {
        status = Eof;
        return Eof;
    }
    sinceMark += nread;
    if (ignoreBytes < nread) {
        SHA1_Update(&sha1, start+ignoreBytes, nread-ignoreBytes);
        ignoreBytes = 0;
    } else {
        ignoreBytes -= nread;
    }
    return nread;
}
StreamStatus
DigestInputStream::skip(int64_t ntoskip, int64_t* skipped) {
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    return InputStream::skip(ntoskip, skipped);
}
StreamStatus
DigestInputStream::mark(int32_t readlimit) {
    if (status) return status;
    sinceMark = 0;
    return input->mark(readlimit);
}
StreamStatus
DigestInputStream::reset() {
    StreamStatus s = input->reset();
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
