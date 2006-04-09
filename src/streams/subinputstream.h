#ifndef SUBINPUTSTREAM_H
#define SUBINPUTSTREAM_H

#include "inputstream.h"

class SubInputStream : public InputStream {
private:
    const int32_t size;
    int32_t left;
    int32_t markleft;
    InputStream *input;
public:
    SubInputStream(InputStream *input, int32_t size);
    Status read(const char*& start, int32_t& nread, int32_t max = 0);
    Status mark(int32_t readlimit);
    Status reset();
    Status skipToEnd();
    int32_t pos() {
        return size-left;
    }
    int32_t getSize() const {
        return size;
    }
};

#endif
