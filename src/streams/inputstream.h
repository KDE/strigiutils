#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <string>

class InputStream {
public:
    enum Status {Ok, Eof, Error};
protected:
    std::string error;
    Status status; // 0: all ok, -1: EOF, -2 error
public:
    virtual ~InputStream() {}
    const std::string& getError() const {
        return error;
    }
    /**
     * Reads input data and points the input pointers to the read
     * data.
     * If the end of stream is reached, -1 is returned.
     * If an error occured, -2 is returned.
     * No more than @param max data wil be read. If @param max == 0, there's no limit.
     **/
    virtual Status read(const char*& start, size_t& read, size_t max = 0) = 0;
    /**
     * Skip @param ntoskip bytes. Unless an error occurs or the end of file is
     * encountered, this amount of bytes is skipped.
     * If the end of stream is reached, -1 is returned.
     * If an error occured, -2 is returned.
     **/
    virtual Status skip(size_t ntoskip);
    virtual Status mark(size_t readlimit) = 0;
    virtual Status reset() = 0;
};

#endif
