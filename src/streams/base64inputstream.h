#ifndef BASE64INPUTSTREAM_H
#define BASE64INPUTSTREAM_H

#include "bufferedstream.h"

namespace jstreams {
class Base64InputStream : public BufferedInputStream<char> {
private:
    int32_t bits;
    char bytestodo;
    char char_count;
    const char* pos, * pend;
    int32_t nleft;

    StreamBase<char>* input;

    static const unsigned char alphabet[];
    static bool inalphabet[256];
    static unsigned char decoder[133];

    bool moreData();
public:
    Base64InputStream(StreamBase<char>* i);
    int32_t fillBuffer(char* start, int32_t space);
};
}

#endif
