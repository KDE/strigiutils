#ifndef DIGESTINPUTSTREAM_H
#define DIGESTINPUTSTREAM_H
#include "streambase.h"
#include <openssl/sha.h>

namespace jstreams {
class DigestInputStream : public StreamBase<char> {
private:
    int32_t ignoreBytes;
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    StreamBase<char> *input;
public:
    DigestInputStream(StreamBase<char> *input);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t);
    void printDigest();
};
}
#endif
