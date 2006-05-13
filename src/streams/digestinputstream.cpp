#include "digestinputstream.h"
using namespace jstreams;

DigestInputStream::DigestInputStream(StreamBase<char> *input) {
    this->input = input;
    size = input->getSize();
    status = Ok;
    ignoreBytes = 0;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
    position = input->getPosition();
    //printf("digread %p %p %i %lli ", this, start, nread, position);
    if (nread < -1) {
        error = input->getError();
        status = Error;
        return -2;
    }
//    if (nread > 0) position += nread;
    if (nread < min) {
        status = Eof;
    }
    if (ignoreBytes < nread) {
        SHA1_Update(&sha1, start+ignoreBytes, nread-ignoreBytes);
        ignoreBytes = 0;
    } else {
        ignoreBytes -= nread;
    }
    //printf("%lli %lli\n", position, getPosition());
    return nread;
}
int64_t
DigestInputStream::skip(int64_t ntoskip) {
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    int64_t skipped = input->skip(ntoskip);
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
    if (status) return status;
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
//    printf("dg newpos %p %lli\n", this, newpos);
    if (newpos < 0) {
        status = Error;
        error = input->getError();
    } else {
        if (newpos < position) {
            ignoreBytes += position - newpos;
        }
        status = (position == size) ?Eof :Ok;
    }
    position = newpos;
//    const char*x; input->read(x,1,0);printf("digread %p\n", x);
//    input->reset(np);
    //printf("%p reset %lli\n", this, newpos);
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
