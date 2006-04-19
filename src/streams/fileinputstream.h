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
    void fillBuffer();
//    char skip(int32_t ntoskip);
};

} // end namespace jstreams

#endif

/**
 * if markPos is not set, we can best write data at the start of the buffer.
 * if markPos is set, we must continue writing where we left off (curPos+avail).
 * we need a function that returns pointers to the places where we want to write to
 **/
