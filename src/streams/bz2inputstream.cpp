#include "bz2inputstream.h"

BZ2InputStream::BZ2InputStream(InputStream *input) {
    // initialize values that signal state
    status = Ok;
    finishedInflating = false;
    this->input = input;

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
    // signal that we need to read into the buffer
    bzstream.avail_out = 1;
}
BZ2InputStream::~BZ2InputStream() {
    dealloc();
}
void
BZ2InputStream::dealloc() {
    BZ2_bzDecompressEnd(&bzstream);
}
InputStream::Status
BZ2InputStream::read(const char*& start, size_t& nread, size_t max) {
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
    return Ok;
}
void
BZ2InputStream::readFromStream() {
    // read data from the input stream
    const char* inStart;
    size_t nread;
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
InputStream::Status
BZ2InputStream::mark(size_t readlimit) {
    buffer.mark(readlimit);
    return Ok;
}
InputStream::Status
BZ2InputStream::reset() {
    if (buffer.markPos) {
        buffer.reset();
        return Ok;
    } else {
        error = "No valid mark for reset.";
        return Error;
    }
}
