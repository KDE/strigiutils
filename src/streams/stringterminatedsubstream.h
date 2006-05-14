#ifndef STRINGTERMINATEDSUBSTREAM
#define STRINGTERMINATEDSUBSTREAM

#include "streambase.h"
#include "kmpsearcher.h"

namespace jstreams {

class StringTerminatedSubStream : public StreamBase<char> {
private:
    const int64_t offset;
    StreamBase<char>* input;
    KMPSearcher searcher;
public:
    StringTerminatedSubStream(StreamBase<char>* i, const std::string& terminator)
        : offset(i->getPosition()), input(i) {
        searcher.setQuery(terminator);
    }
    int32_t read(const char*& start, int32_t min=0, int32_t max=0);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t);
    int64_t getOffset() const { return offset; }
};

}

#endif
