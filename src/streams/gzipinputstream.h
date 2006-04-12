#ifndef GZIPINPUTSTREAM_H
#define GZIPINPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

struct z_stream_s;

namespace jstreams {

class GZipInputStream : public InputStream {
private:
    bool finishedInflating;
    z_stream_s *zstream;
    InputStream *input;
    InputStreamBuffer<char> buffer;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
public:
    enum ZipFormat { ZLIBFORMAT, GZIPFORMAT, ZIPFORMAT};
    GZipInputStream(InputStream *input, ZipFormat format=GZIPFORMAT);
    ~GZipInputStream();
    void restart(InputStream *input);
    StreamStatus read(const char*& start, int32_t& nread, int32_t max = 0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

} // end namespace jstreams

#endif
