#ifndef SUBINPUTSTREAM_H
#define SUBINPUTSTREAM_H

#include "inputstream.h"

class SubInputStream : public InputStream {
private:
    const size_t size;
    size_t left;
    size_t markleft;
    InputStream *input;
public:
    SubInputStream(InputStream *input, size_t size);
    Status read(const char*& start, size_t& nread, size_t max = 0);
    Status mark(size_t readlimit);
    Status reset();
    Status skipToEnd();
    size_t pos() {
        return size-left;
    }
    size_t getSize() const {
        return size;
    }
};

#endif
