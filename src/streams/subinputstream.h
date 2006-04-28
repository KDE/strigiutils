#ifndef SUBINPUTSTREAM_H
#define SUBINPUTSTREAM_H

#include "streambase.h"

namespace jstreams {

class SubInputStream : public StreamBase<char> {
private:
    int64_t markPos;
    StreamBase<char> *input;
public:
    SubInputStream(StreamBase<char> *input, int64_t size);
    int32_t read(const char*& start, int32_t min, int32_t max);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
    int64_t skip(int64_t ntoskip);
};

} //end namespace jstreams

#endif
