#ifndef GZIPINPUTSTREAM_H
#define GZIPINPUTSTREAM_H

#include "bufferedstream.h"

struct z_stream_s;

namespace jstreams {

class GZipInputStream : public BufferedInputStream<char> {
private:
    z_stream_s* zstream;
    StreamBase<char>* input;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
public:
    enum ZipFormat { ZLIBFORMAT, GZIPFORMAT, ZIPFORMAT};
    GZipInputStream(StreamBase<char>* input, ZipFormat format=GZIPFORMAT);
    ~GZipInputStream();
    bool fillBuffer();
};

} // end namespace jstreams

#endif
