#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include <string>
#include <iconv.h>
#include "bufferedstream.h"

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
    int32_t charsLeft;

    InputStreamBuffer<char> charbuf;
    void readFromStream();
    int32_t decode(wchar_t* start, int32_t space);
public:
    InputStreamReader(StreamBase<char> *i, const char *enc=0);
    ~InputStreamReader();
    int32_t fillBuffer(wchar_t* start, int32_t space);
};

} // end namespace jstreams

#endif
