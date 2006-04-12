#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include <string>
#include <iconv.h>
#include "fileinputstream.h"
#include "Reader.h"

namespace jstreams {

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
    using Reader::read;
    StreamStatus read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

class FileReader : public Reader {
    FileInputStream* input;
    InputStreamReader* reader;
public:
    FileReader(const char* fname, const char* encoding_scheme=NULL,
        const int32_t cachelen = 13,
        const int32_t cachebuff = 14 );
    ~FileReader();
    using Reader::read;
    StreamStatus read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
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
    StreamStatus read(wchar_t&);
    StreamStatus read(const wchar_t*& start, int32_t& nread, int32_t max=0);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

} // end namespace jstreams

#endif
