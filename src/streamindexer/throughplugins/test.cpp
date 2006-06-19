#include <pluginthroughanalyzer.h>

/* test class 1 */
class Analyzer1 : public jstreams::StreamThroughAnalyzer {
private:
    jstreams::Indexable* idx;
public:
    void setIndexable(jstreams::Indexable*i) {
        idx = i;
    }
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in) {
        idx->setField("hi", "hi");
        return in;
    }
};

/* test class 2 */
class Analyzer2 : public jstreams::StreamThroughAnalyzer {
    void setIndexable(jstreams::Indexable*) {}
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in) {
        return in;
    }
};

REGISTER_THROUGHANALYZER(Analyzer1)
REGISTER_THROUGHANALYZER(Analyzer2)
