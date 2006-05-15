#ifndef SUBINPUTSTREAM_H
#define SUBINPUTSTREAM_H

#include "streambase.h"

namespace jstreams {

class SubInputStream : public StreamBase<char> {
private:
    const int64_t offset;
    StreamBase<char> *input;
public:
    SubInputStream(StreamBase<char> *input, int64_t size=-1);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t newpos);
    int64_t skip(int64_t ntoskip);
};

} //end namespace jstreams

#endif
