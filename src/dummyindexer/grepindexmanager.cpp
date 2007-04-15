#include "grepindexmanager.h"
#include "grepindexreader.h"
#include "indexwriter.h"
using namespace Strigi;

/**
 * Dummy class that does not write anything.
 **/
class DummyGrepIndexWriter : public Strigi::IndexWriter {
public:
    DummyGrepIndexWriter() {}
    void startAnalysis(const AnalysisResult*) {}
    void addText(const AnalysisResult* result, const char* text,
        int32_t length) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const unsigned char* data, uint32_t size) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        int32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        uint32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        double value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& name, const std::string& value) {}
    void finishAnalysis(const AnalysisResult* result) {}
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};
GrepIndexManager::GrepIndexManager(const char* path)
        :reader(0),
         writer(new DummyGrepIndexWriter()){
}
GrepIndexManager::~GrepIndexManager() {
    delete writer;
}
Strigi::IndexReader*
GrepIndexManager::indexReader() {
    return reader;
}
Strigi::IndexWriter*
GrepIndexManager::indexWriter() {
    return writer;
}
Strigi::IndexManager*
createGrepIndexManager(const char* path) {
    return new GrepIndexManager(path);
}
