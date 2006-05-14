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
        return nread;
    }
    if (nread < -1) {
        status = Error;
        error = input->getError();
        return nread;
    }

    const char* end = searcher.search(start, nread);
    if (end) {
//        printf("THE END %p %p %s\n", start, end, searcher.getQuery().c_str());
//        printf("TE %i '%.*s'\n", end-start, 10, end);
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
//        printf("ehh %i %i\n", status, max);
    } else if (max != 0 && nread > max) {
//        printf("max\n");
        // we are near the end of the stream but cannot pass all data
        // at once because the amount read is larger than the amount to pass
        input->reset(pos + max);
        nread = max;
    } else {
//        printf("huh????\n");
        // we are at the end of the stream, so no need to rewind
        // signal the end of stream at the next call
        status = Eof;
    }
//    printf("stss: %i '%.*s'\n", nread, (20>nread)?nread:20, start);

    position += nread;
    if (status == Eof) {
        size = position;
    }
    return nread;
}
int64_t
StringTerminatedSubStream::mark(int32_t readlimit) {
    return input->mark(readlimit) - offset;
}
int64_t
StringTerminatedSubStream::reset(int64_t newpos) {
    printf("stssreset %lli\n", newpos);
    position = input->reset(newpos+offset);
    if (position >= offset) {
        position -= offset;
    } else {
        // the stream is not positioned at a valid position
        status = Error;
        position = -1;
    }
    return position;
}
