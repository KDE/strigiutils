#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include <CLucene/clucene-config.h>
#include <CLucene.h>

class InputStream;

class InputStreamReader : public lucene::util::Reader {
private:
    InputStream *input;
    int64_t pos;
public:
    InputStreamReader(InputStream *input);
    void close();
    Status read(const TCHAR*& start, int32_t& nread, int32_t max=0);
};

#endif
