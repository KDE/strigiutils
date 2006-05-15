#include "digestinputstream.h"
#include <cassert>
using namespace std;
using namespace jstreams;

DigestInputStream::DigestInputStream(StreamBase<char> *input) {
    this->input = input;
    size = input->getSize();
#ifndef NDEBUG
    totalread = 0;
#endif
    status = Ok;
    ignoreBytes = 0;
    finished = false;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
//    printf("read min %i max %i nread %i\n", min, max, nread);
    position = input->getPosition();
    if (nread < -1) {
        error = input->getError();
        status = Error;
        return -2;
    }
    if (ignoreBytes < nread) {
        SHA1_Update(&sha1, start+ignoreBytes, nread-ignoreBytes);
#ifndef NDEBUG
        totalread += nread-ignoreBytes;
#endif
        ignoreBytes = 0;
    } else {
        ignoreBytes -= nread;
    }
    if (nread < min) {
        status = Eof;
        if (size == -1) {
            size = position;
        }
#ifndef NDEBUG
        if (size != position || size != totalread) {
            printf("size: %lli position: %lli totalread: %lli\n",
                size, position, totalread);
        }
        assert(size == position);
        assert(totalread == size);
#endif
        finishDigest();
    }
    return nread;
}
/*int64_t
DigestInputStream::skip(int64_t ntoskip) {
//    printf("skipping %lli\n", ntoskip);
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    int64_t skipped = StreamBase<char>::skip(ntoskip);

    return skipped;
}*/
int64_t
DigestInputStream::mark(int32_t readlimit) {
    if (status == Error) return -2;
    return input->mark(readlimit);
}
int64_t
DigestInputStream::reset(int64_t np) {
//    printf("reset from %lli to %lli\n", position, np);
    if (np > position) {
        // advance to the new position, using skip ensure we actually read
        // the files
        skip(np - position);
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
    if (finished) {
        printf("The hash %p: %s\n", this, getDigestString().c_str());
    } else {
        printf("The digest is not ready yet.\n");
    }
}
void
DigestInputStream::finishDigest() {
    if (finished) return;
#ifndef NDEBUG
//    printf("finishing digest totalread: %lli size %lli\n", totalread, size);
    assert(totalread == size);
#endif
    SHA1_Final(digest, &sha1);
    finished = true;
#ifndef NDEBUG
//    printDigest();
#endif
}
string
DigestInputStream::getDigestString() {
    if (finished) {
        char d[41];
        for (int i = 0; i < 20; i++) {
            sprintf(d+2*i, "%02x", digest[i]);
        }
        return d;
    }
    return "";
}
