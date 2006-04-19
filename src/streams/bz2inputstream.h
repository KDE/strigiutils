#ifndef BZ2INPUTSTREAM_H
#define BZ2INPUTSTREAM_H

#include "bufferedstream.h"

#include <bzlib.h>

namespace jstreams {

class BZ2InputStream : public BufferedInputStream<char> {
private:
    bool allocatedBz;
    bool finishedInflating;
    bz_stream bzstream;
    StreamBase<char> *input;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
protected:
    void fillBuffer();
public:
    BZ2InputStream(StreamBase<char>* input);
    ~BZ2InputStream();
    static bool checkHeader(const char* data, int32_t datasize);
};

} // end namespace jstreams

#endif
