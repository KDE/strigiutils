#ifndef STRINGREADER_H
#define STRINGREADER_H

#include "inputstreamreader.h"

namespace jstreams {

class StringReader : public StreamBase<wchar_t> {
private:
    wchar_t* data;
    int64_t markpt;
public:
    StringReader ( const wchar_t* value );
    StringReader ( const wchar_t* value, const int32_t length );
    ~StringReader();
    int32_t read(const wchar_t*& start);
    int32_t read(const wchar_t*& start, int32_t ntoread);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

} // end namespace jstreams

#endif
