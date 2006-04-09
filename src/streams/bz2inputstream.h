#ifndef BZ2INPUTSTREAM_H
#define BZ2INPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

#include <bzlib.h>

class BZ2InputStream : public InputStream {
private:
    bool allocatedBz;
    bool finishedInflating;
    bz_stream bzstream;
    InputStream *input;
    InputStreamBuffer<char> buffer;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
public:
    BZ2InputStream(InputStream *input);
    ~BZ2InputStream();
//    void restart(InputStream *input);
    Status read(const char*& start, int32_t& nread, int32_t max = 0);
    Status mark(int32_t readlimit);
    Status reset();
};

#endif
