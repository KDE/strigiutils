#include <pluginthroughanalyzer.h>

/* test class 1 */
class Analyzer1 : public jstreams::StreamThroughAnalyzer {
public:
    void setIndexable(jstreams::Indexable*) {}
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in) {
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
