#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

class FileInputStream : public InputStream {
private:
    FILE *file;
    std::string filepath;
    InputStreamBuffer<char> buffer;

    void readFromFile();
public:
    static const size_t defaultBufferSize;
    FileInputStream(const char *filepath, size_t buffersize=defaultBufferSize);
    ~FileInputStream();
    Status read(const char*& start, size_t& nread, size_t max = 0);
    Status mark(size_t readlimit);
    Status reset();
//    char skip(size_t ntoskip);
};

#endif

/**
 * if markPos is not set, we can best write data at the start of the buffer.
 * if markPos is set, we must continue writing where we left off (curPos+avail).
 * we need a function that returns pointers to the places where we want to write to
 **/
