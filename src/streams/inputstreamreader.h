#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include <string>
#include <iconv.h>
#include "fileinputstream.h"
#include "bufferedstream.h"
#include "inputstream.h"

namespace jstreams {

/**
 * Info on conversions:
http://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html
http://tangentsoft.net/mysql++/doc/userman/html/unicode.html
 **/
class InputStreamReader : public BufferedInputStream<wchar_t> {
private:
    iconv_t converter;
    bool finishedDecoding;
    StreamBase<char>* input;
    const char* inStart;
    int32_t inSize;
    int32_t charsLeft;

    InputStreamBuffer<char> charbuf;
    void readFromStream();
    int32_t decode(wchar_t* start, int32_t space);
public:
    InputStreamReader(StreamBase<char> *i, const char *enc=0);
    ~InputStreamReader();
    int32_t fillBuffer(wchar_t* start, int32_t space);
};

class FileReader : public StreamBase<wchar_t> {
    FileInputStream* input;
    InputStreamReader* reader;
public:
    FileReader(const char* fname, const char* encoding_scheme=NULL,
        const int32_t cachelen = 13,
        const int32_t cachebuff = 14 );
    ~FileReader();
    int32_t read(const wchar_t*& start);
    int32_t read(const wchar_t*& start, int32_t ntoread);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

class StringReader : public StreamBase<wchar_t> {
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
