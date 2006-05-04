#include "stringterminatedsubstream.h"
using namespace jstreams;

int32_t
StringTerminatedSubStream::read(const char*& start, int32_t min, int32_t max) {
    if (status == Eof) return -1;
    if (status == Error) return -2;

    // convenience parameter
    int32_t tl = searcher.getQueryLength();

    // increase min and max to accomodate for the length of the terminator
    int32_t tlmin = min;
    int32_t tlmax = max;
    if (tlmin == 0) {
        tlmin = 1 + tlmin;
    } else {
        tlmin += tl;
    }
    if (tlmax > 0 && tlmax < tlmin) tlmax = tlmin;

    int64_t pos = input->getPosition();
    int32_t nread = input->read(start, tlmin, tlmax);
    if (nread == -1) {
        status = Eof;
        printf("%i\n", nread);
        return nread;
    }
    if (nread < -1) {
        status = Error;
        error = input->getError();
        return nread;
    }

    printf("'%*s'\n", nread, start);
    const char* end = searcher.search(start, nread);
    if (end) {
        nread = end - start;
        // signal the end of stream at the next call
        status = Eof;
        // set input stream to point after the terminator
        input->reset(pos + nread + tl);
    } else if (nread > tl) {
        // we are not at or near the end and read the required amount
        // reserve the last bit of buffer for rereading to match the terminator
        // in the next call
        nread -= tl;
        // we rewind, but the pointer 'start' will stay valid nontheless
        input->reset(pos + nread);
    } else if (max != 0 && nread > max) {
        // we are near the end of the stream but cannot pass all data
        // at once because the amount read is larger than the amount to pass
        input->reset(pos + max);
        nread = max;
    } else {
        // we are at the end of the stream, so no need to rewind
        // signal the end of stream at the next call
        status = Eof;
    }

    return nread;
}
int64_t
StringTerminatedSubStream::mark(int32_t readlimit) {
    return input->mark(readlimit);
}
int64_t
StringTerminatedSubStream::reset(int64_t newpos) {
    return input->reset(newpos);
}
