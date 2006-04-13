#include "bz2inputstream.h"
using namespace jstreams;

bool
BZ2InputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x42, 0x5a, 0x68, 0x39, 0x31};
    if (datasize < 5) return false;
    return memcmp(data, magic, 5) == 0;
}
BZ2InputStream::BZ2InputStream(InputStream *input) {
    // initialize values that signal state
    status = Ok;
    finishedInflating = false;
    this->input = input;

    // TODO: check first bytes of stream before allocating buffer
    // 0x42 0x5a 0x68 0x39 0x31
    if (!checkMagic()) {
        error = "Magic bytes are wrong.";
        status = Error;
        allocatedBz = false;
        return;
    }

    // initialize the output buffer
    buffer.setSize(262144);

    bzstream.bzalloc = NULL;
    bzstream.bzfree = NULL;
    bzstream.opaque = NULL;
    bzstream.avail_in = 0;
    bzstream.next_in = NULL;
    int r;
    r = BZ2_bzDecompressInit(&bzstream, 0, 0);
    if (r != BZ_OK) {
        error = "Error initializing BZ2InputStream.";
        printf("Error initializing BZ2InputStream.\n");
        dealloc();
        status = Error;
        return;
    }
    allocatedBz = true;
    // signal that we need to read into the buffer
    bzstream.avail_out = 1;
}
BZ2InputStream::~BZ2InputStream() {
    dealloc();
}
void
BZ2InputStream::dealloc() {
    if (allocatedBz) {
        BZ2_bzDecompressEnd(&bzstream);
    }
}
bool
BZ2InputStream::checkMagic() {
    input->mark(5);
    const char* begin = 0;
    const char* ptr;
    int32_t nread;
    int32_t total = 0;
    do {
        status = input->read(ptr, nread, 5-total);
        if (status != Ok) {
            error = input->getError();
            return false;
        }
        if (begin == 0) {
            begin = ptr;
        }
        total += nread;
    } while (total < 5);
    input->reset();

    return checkHeader(begin, 5);
}
StreamStatus
BZ2InputStream::read(const char*& start, int32_t& nread, int32_t max) {
    // if an error occured earlier, signal this
    if (status) return status;

    // if we cannot read and there's nothing in the buffer
    // (this can maybe be fixed by calling reset)
    if (finishedInflating && buffer.avail == 0) return Eof;

    // check if there is still data in the buffer
    if (buffer.avail == 0) {
        decompressFromStream();
        if (status) return status;
    }

    // set the pointers to the available data
    buffer.read(start, nread, max);
    if (nread == 0) {
        return read(start, nread, max);
    }
    return Ok;
}
void
BZ2InputStream::readFromStream() {
    // read data from the input stream
    const char* inStart;
    int32_t nread;
    status = input->read(inStart, nread);
    if (status == Error) {
        error = "Error reading bz2: " + input->getError();
    }
    bzstream.next_in = (char*)inStart;
    bzstream.avail_in = nread;
}
void
BZ2InputStream::decompressFromStream() {
    // make sure there is data to decompress
    if (bzstream.avail_out != 0) {
        readFromStream();
        if (status) {
            // no data was read
            return;
        }
    }
    // make sure we can write into the buffer
    int space = buffer.getWriteSpace();
    bzstream.avail_out = space;
    bzstream.next_out = buffer.curPos;
    // decompress
    int r = BZ2_bzDecompress(&bzstream);
    // inform the buffer of the number of bytes that was read
    buffer.avail = space - bzstream.avail_out;
    switch (r) {
    case BZ_PARAM_ERROR:
    case BZ_DATA_ERROR:
    case BZ_DATA_ERROR_MAGIC:
    case BZ_MEM_ERROR:
        error = "Error while inflating bz2 stream.";
        status = Error;
        break;
    case BZ_STREAM_END:
        // we are finished decompressing,
        // (but this stream is not yet finished)
        finishedInflating = true;
        break;
    }
}
StreamStatus
BZ2InputStream::mark(int32_t readlimit) {
    buffer.mark(readlimit);
    return Ok;
}
StreamStatus
BZ2InputStream::reset() {
    if (buffer.markPos) {
        buffer.reset();
        return Ok;
    } else {
        error = "No valid mark for reset.";
        return Error;
    }
}
