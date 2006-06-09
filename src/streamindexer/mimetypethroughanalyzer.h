#ifndef MIMETYPETHROUGHANALYZER_H
#define MIMETYPETHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
extern "C" {
    #include <magic.h>
}
class MimeTypeThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    magic_t magic;
    jstreams::Indexable* idx;
public:
    MimeTypeThroughAnalyzer();
    ~MimeTypeThroughAnalyzer();
    void setIndexable(jstreams::Indexable* i) { idx = i; }
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

#endif
