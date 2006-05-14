#include "digestinputstream.h"
using namespace std;
using namespace jstreams;

DigestInputStream::DigestInputStream(StreamBase<char> *input) {
    this->input = input;
    size = input->getSize();
    status = Ok;
    ignoreBytes = 0;
    finished = false;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
    position = input->getPosition();
    if (nread < -1) {
        error = input->getError();
        status = Error;
        return -2;
    }
    if (nread < min) {
        status = Eof;
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
    int64_t skipped = StreamBase<char>::skip(ntoskip);
    status = input->getStatus();
    if (status == Error) {
        error = input->getError();
        position = -1;
    } else {
        position += skipped;
    }

    return skipped;
}
int64_t
DigestInputStream::mark(int32_t readlimit) {
    if (status == Error) return -2;
    return input->mark(readlimit);
}
int64_t
DigestInputStream::reset(int64_t np) {
    if (np > position) {
        // we cannot just skip, but must read this
        skip(np-position);
        return position;
    }
    int64_t newpos = input->reset(np);
    if (newpos < 0) {
        status = Error;
        error = input->getError();
    } else {
        if (newpos < position) {
            ignoreBytes += position - newpos;
        }
        status = (newpos == size) ?Eof :Ok;
    }
    position = newpos;
    return newpos;
}
void
DigestInputStream::printDigest() {
    finishDigest();
    printf("The hash %p: %s\n", this, getDigestString().c_str());
/*    for (int i = 0; i < 20; i++) {
        printf("%02x ", digest[i]);
    }
    printf("\n");*/
}
void
DigestInputStream::finishDigest() {
    if (!finished) {
        SHA1_Final(digest, &sha1);
        finished = true;
    }
}
string
DigestInputStream::getDigestString() {
    finishDigest();
    char d[41];
    for (int i = 0; i < 20; i++) {
        sprintf(d+2*i, "%02x", digest[i]);
    }
    return d;
}
