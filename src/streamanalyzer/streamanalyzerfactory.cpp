#include "streamanalyzerfactory.h"
using namespace Strigi;
using namespace std;

class StreamAnalyzerFactory::Private {
public:
    vector<const RegisteredField*> fields;
};

StreamAnalyzerFactory::StreamAnalyzerFactory() : p(new Private()) {}

StreamAnalyzerFactory::~StreamAnalyzerFactory() {
    delete p;
}
const std::vector<const RegisteredField*>&
StreamAnalyzerFactory::registeredFields() const {
    return p->fields;
}
void
StreamAnalyzerFactory::addField(const RegisteredField* f) {
    p->fields.push_back(f);
}
