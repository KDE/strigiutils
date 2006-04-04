#ifndef GZIPINPUTSTREAM_H
#define GZIPINPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

struct z_stream_s;
class GZipInputStream : public InputStream {
private:
    bool finishedInflating;
    z_stream_s *zstream;
    InputStream *input;
    InputStreamBuffer<char> buffer;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
public:
    enum ZipFormat { ZLIBFORMAT, GZIPFORMAT, ZIPFORMAT};
    GZipInputStream(InputStream *input, ZipFormat format=GZIPFORMAT);
    ~GZipInputStream();
    void restart(InputStream *input);
    Status read(const char*& start, size_t& nread, size_t max = 0);
    Status mark(size_t readlimit);
    Status reset();
};

#endif
