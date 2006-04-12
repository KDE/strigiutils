#ifndef BZ2INPUTSTREAM_H
#define BZ2INPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

#include <bzlib.h>

namespace jstreams {

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
    StreamStatus read(const char*& start, int32_t& nread, int32_t max = 0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

} // end namespace jstreams

#endif
