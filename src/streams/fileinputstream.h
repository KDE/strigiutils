#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "bufferedstream.h"

namespace jstreams {

class FileInputStream : public BufferedInputStream<char> {
private:
    FILE *file;
    std::string filepath;

public:
    static const int32_t defaultBufferSize;
    FileInputStream(const char *filepath, int32_t buffersize=defaultBufferSize);
    ~FileInputStream();
    int32_t fillBuffer(char* start, int32_t space);
};

} // end namespace jstreams

#endif

