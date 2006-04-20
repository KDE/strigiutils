#include "bz2inputstream.h"
using namespace jstreams;

bool
BZ2InputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x42, 0x5a, 0x68, 0x39, 0x31};
    if (datasize < 5) return false;
    return memcmp(data, magic, 5) == 0;
}
BZ2InputStream::BZ2InputStream(StreamBase<char>* input) {
    // initialize values that signal state
    status = Ok;
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
        nread = input->read(ptr, 5-total);
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
/*int32_t
BZ2InputStream::read(const char*& start, int32_t ntoread) {
    // if an error occured earlier, signal this
    if (status == Error) return -1;
    if (status == Eof) return 0;

    // if we cannot read and there's nothing in the buffer
    // (this can maybe be fixed by calling reset)
    if (finishedInflating && buffer.avail == 0) return 0;

    // check if there is still data in the buffer
    if (buffer.avail == 0) {
        decompressFromStream();
        if (status == Error) return -1;
        if (status == Eof) return 0;
    }

    // set the pointers to the available data
    int32_t nread = buffer.read(start, ntoread);
    while (nread == 0) {
        nread = read(start, ntoread);
    }
    return nread;
} */
void
BZ2InputStream::readFromStream() {
    // read data from the input stream
    const char* inStart;
    int32_t nread;
    nread = input->read(inStart);
    if (status == Error) {
        error = "Error reading bz2: ";
        error += input->getError();
    }
    bzstream.next_in = (char*)inStart;
    bzstream.avail_in = nread;
}
bool
BZ2InputStream::fillBuffer() {
    // make sure there is data to decompress
    if (bzstream.avail_out != 0) {
        readFromStream();
        if (status != Ok) {
            // no data was read
            return false;
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
        return false;
    case BZ_STREAM_END:
        // we are finished decompressing,
        // (but this stream is not yet finished)
        return false;
    }
    return true;
}
