#include "digestthroughanalyzer.h"
#include "inputstream.h"
#include <openssl/sha.h>
using namespace jstreams;

class DigestInputStream : public InputStream {
private:
    int32_t ignoreBytes;
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    InputStream *input;
public:
    DigestInputStream(InputStream *input);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t);
    void printDigest();
};
DigestInputStream::DigestInputStream(InputStream *input) {
    this->input = input;
    status = Ok;
    ignoreBytes = 0;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
    if (nread == -1) {
        error = input->getError();
        status = Error;
        return -1;
    }
    if (nread < min) {
        status = Eof;
        return Eof;
    }
    if (ignoreBytes < nread) {
        SHA1_Update(&sha1, start+ignoreBytes, nread-ignoreBytes);
        ignoreBytes = 0;
    } else {
        ignoreBytes -= nread;
    }
    return nread;
}
int64_t
DigestInputStream::skip(int64_t ntoskip) {
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    return input->skip(ntoskip);
}
int64_t
DigestInputStream::mark(int32_t readlimit) {
    if (status) return status;
    return input->mark(readlimit);
}
int64_t
DigestInputStream::reset(int64_t newpos) {
    newpos = input->reset(newpos);
    if (newpos < 0) {
        status = Error;
        error = input->getError();
    } else {
        if (newpos < position) {
            ignoreBytes += position - newpos;
        }
    }
    position = newpos;
    return newpos;
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
DigestThroughAnalyzer::setIndexable(jstreams::Indexable*) {
}
