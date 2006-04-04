#ifndef BZ2INPUTSTREAM_H
#define BZ2INPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

#include <bzlib.h>

class BZ2InputStream : public InputStream {
private:
    bool finishedInflating;
    bz_stream bzstream;
    InputStream *input;
    InputStreamBuffer<char> buffer;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
public:
    BZ2InputStream(InputStream *input);
    ~BZ2InputStream();
//    void restart(InputStream *input);
    Status read(const char*& start, size_t& nread, size_t max = 0);
    Status mark(size_t readlimit);
    Status reset();
};

#endif
