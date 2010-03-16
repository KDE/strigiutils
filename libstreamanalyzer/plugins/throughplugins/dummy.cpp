#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"

#include <list>

using namespace Strigi;

// Dummy factory
class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        return std::list<StreamThroughAnalyzerFactory*>();
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
