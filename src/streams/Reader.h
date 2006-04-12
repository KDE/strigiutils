#ifndef READER_H
#define READER_H

#include <string>

// java mapping: long=int64, int=int32, byte=uint8_t
class Reader {
public:
    enum Status { Ok, Eof, Error };
protected:
    Status status;
    std::string error;
public:
    virtual ~Reader(){}
    const std::string& getError() const {
        return error;
    }
    virtual void close() {};
    virtual Status read(wchar_t&);
    /**
     * nread is the number of _characters_ read.
     * max is the maximal number of characters to read
     **/
    virtual Status read(const wchar_t*& start, int32_t& nread, int32_t max=0)= 0;
    /* the available value may be greater than the actual value if
      the encoding is a variable one (such as utf8 or unicode) */
    /**
     * Skip @param ntoskip bytes. Unless an error occurs or the end of file is
     * encountered, this amount of bytes is skipped.
     * If the end of stream is reached, -1 is returned.
     * If an error occured, -2 is returned.
     **/
    virtual Status skip(int32_t ntoskip);
    virtual Status mark(int32_t readlimit) = 0;
    virtual Status reset() = 0;
};
#endif
