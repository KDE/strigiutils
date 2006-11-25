#include <list>

namespace jstreams {
class StreamEndAnalyzerFactory;
class StreamThroughAnalyzerFactory;
class AnalyzerFactoryFactory {
public:
    virtual ~AnalyzerFactoryFactory() {}
    virtual std::list<StreamEndAnalyzerFactory*>
            getStreamEndAnalyzerFactories() const {
        std::list<StreamEndAnalyzerFactory*> af;
        return af;
    }
    virtual std::list<StreamThroughAnalyzerFactory*>
            getStreamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        return af;
    }
};
}
