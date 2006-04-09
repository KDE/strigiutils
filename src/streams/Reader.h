#ifndef READER_H
#define READER_H

#include <string>
#include <iconv.h>
#include "fileinputstream.h"

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
/**
 * Info on conversions:
http://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html
http://tangentsoft.net/mysql++/doc/userman/html/unicode.html
 **/
class InputStreamReader : public Reader {
private:
    iconv_t converter;
    bool finishedDecoding;
    InputStream* input;
    const char* inStart;
    int32_t inSize;
    int32_t charsLeft;

    InputStreamBuffer<char> charbuf;
    InputStreamBuffer<wchar_t> buffer;
    void readFromStream();
    void decodeFromStream();
    void decode();
public:
    InputStreamReader(InputStream *i, const char *enc=NULL);
    ~InputStreamReader();
    Status read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    Status mark(int32_t readlimit);
    Status reset();
};

class FileReader : public Reader {
    FileInputStream* input;
    InputStreamReader* reader;
public:
    FileReader(const char* fname, const char* encoding_scheme=NULL,
        const int32_t cachelen = 13,
        const int32_t cachebuff = 14 );
    ~FileReader();
    Status read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    Status mark(int32_t readlimit);
    Status reset();
};

class StringReader:public Reader{
private:
    wchar_t* data;
    int32_t pt;
    int32_t markpt;
    int32_t len;
public:
    StringReader ( const wchar_t* value );
    StringReader ( const wchar_t* value, const int32_t length );
    ~StringReader();
    void close();
    Status read(wchar_t&);
    Status read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    Status mark(int32_t readlimit);
    Status reset();
};

#endif
