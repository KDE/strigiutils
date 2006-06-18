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

/* factory for test class 1 */
jstreams::StreamThroughAnalyzer*
createAnalyzer1() {
    return new Analyzer1();
}
/* factory for test class 2 */
jstreams::StreamThroughAnalyzer*
createAnalyzer2() {
    printf("new analyzer\n");
    return new Analyzer2();
}

createThroughAnalyzer_t
cTA[] = { createAnalyzer1, createAnalyzer2, 0 };

void
destroy(const jstreams::StreamThroughAnalyzer*a) {
    delete a;
}

destroyThroughAnalyzer_t d = destroy;



extern "C" {
PluginThroughAnalyzer::Factory f(cTA, &destroy);

}

