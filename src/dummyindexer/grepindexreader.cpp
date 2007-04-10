#include "grepindexreader.h"
#include "analyzerconfiguration.h"
#include "query.h"
#include "indexwriter.h"
#include "filelister.h"
#include <set>
using namespace std;
using namespace Strigi;

/**
 * Custom configuration that extracts specific fields.
 **/
class FieldAnalyzerConfiguration : public AnalyzerConfiguration {
private:
    const set<string> neededFields;
    mutable set<string> availableFields;
    char m_maxDepth;
    const bool needsAllFields;
public:
    FieldAnalyzerConfiguration(const set<string>& fields);
    bool useFactory(StreamAnalyzerFactory*) const;
    bool useFactory(StreamEndAnalyzerFactory*) const;
    bool hasAllFields() const;
};
FieldAnalyzerConfiguration::FieldAnalyzerConfiguration(const set<string>& f)
        :neededFields(f), m_maxDepth(-1), needsAllFields(f.find("")!=f.end()) {
}
bool
FieldAnalyzerConfiguration::useFactory(StreamAnalyzerFactory* f) const {
    bool use = needsAllFields;
    const vector<const RegisteredField*>& fields = f->registeredFields();
    vector<const RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        const RegisteredField* field = *i;
        do {
            if (neededFields.find(field->key()) != neededFields.end()) {
                availableFields.insert(field->key());
                use = true;
            }
            field = field->parent();
        } while (field);
    }
    return use;
}
bool
FieldAnalyzerConfiguration::useFactory(StreamEndAnalyzerFactory* f) const {
    return f->analyzesSubStreams() || static_cast<StreamAnalyzerFactory*>(f);
}

class QueryIndexWriter : public IndexWriter {
public:
    void startAnalysis(const AnalysisResult*) {}
    void addText(const AnalysisResult* result, const char* text, int32_t length) {}
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
public:
    void commit() { return; }
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
    /**
     * @brief Return the number of objects that are currently in the cache.
     **/
    virtual int itemsInCache() { return 0; }
    void optimize() {}
    void initWriterData(const Strigi::FieldRegister& fieldRegister) {}
    void releaseWriterData(const Strigi::FieldRegister& fieldRegister) {}
};

class GrepIndexReader::Private {
public:
    const string dir;

    Private(const string& d) :dir(d) {}
};

GrepIndexReader::GrepIndexReader(const string& dir) :p(new Private(dir)) {
}
GrepIndexReader::~GrepIndexReader() {
    delete p;
}
void
getFields(set<string>& fields, const Query& query) {
    if (query.expression().length()) {
        fields.insert(query.fieldName());
    } else {
        list<Query>::const_iterator i;
        for (i = query.terms().begin(); i != query.terms().end(); ++i) {
            getFields(fields, *i);
        }
    }
}
int32_t
GrepIndexReader::countHits(const Query& query) {
    QueryIndexWriter qiw;
    // make an analyzerconfiguration with a limited set of fields
    set<string> fields;
    getFields(fields, query);
    FieldAnalyzerConfiguration conf(fields);
    StreamAnalyzer analyzer(conf);
    analyzer.setIndexWriter(qiw);
    return -1;
}
vector<IndexedDocument>
GrepIndexReader::query(const Query&) {
    vector<IndexedDocument> hits;
    return hits;
}
map<string, time_t>
GrepIndexReader::files(char depth) {
    map<string, time_t> files;
    return files;
}
int32_t
GrepIndexReader::countDocuments() { return -1; }
int32_t
GrepIndexReader::countWords() { return -1; }
int64_t
GrepIndexReader::indexSize() {
    // we have no index :-)
    return 0;
}
/**
 * This does not have to be implemented since we have not index.
 **/
int64_t
GrepIndexReader::documentId(const string& uri) {
    return -1;
}
/**
 * This does not have to be implemented since we have not index.
 **/
time_t
GrepIndexReader::mTime(int64_t docid) {
    return -1;
}
vector<string>
GrepIndexReader::fieldNames() {
    vector<string> fieldnames;
    return fieldnames;
}
vector<pair<string,uint32_t> >
GrepIndexReader::histogram(const string& query, const string& fieldname,
            const string& labeltype) {
    vector<pair<string,uint32_t> > histogram;
    return histogram;
}
int32_t
GrepIndexReader::countKeywords(const string& keywordprefix,
        const vector<string>& fieldnames) {
    return -1;
}
vector<string>
GrepIndexReader::keywords(
        const string& keywordmatch,
        const vector<string>& fieldnames,
        uint32_t max, uint32_t offset) {
    vector<string> keywords;
    return keywords;
}
int
main() {
    GrepIndexReader("/home");
    return 0;
}
