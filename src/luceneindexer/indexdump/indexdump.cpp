#include <CLucene.h>
#include <CLucene/search/QueryFilter.h>
#include <sstream>
#include "cluceneindexmanager.h"

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::Term;
using lucene::index::TermEnum;
using lucene::search::TermQuery;
using lucene::search::WildcardQuery;
using lucene::search::BooleanQuery;
using lucene::search::RangeQuery;
using lucene::search::QueryFilter;
using lucene::search::HitCollector;
using lucene::util::BitSet;
using lucene::document::DocumentFieldEnumeration;
using lucene::index::IndexReader;

void
docdump(Document* doc) {
    DocumentFieldEnumeration* e = doc->fields();
    Field* f = e->nextElement();
    while (f) {
        f = e->nextElement();
    }
}
std::string
t2a(const TCHAR* t) {
    return wchartoutf8(t);
}
void
indexdump(const char* dir) {
    IndexReader* indexreader = IndexReader::open(dir);
    int32_t max = indexreader->maxDoc();
    for (int i=0; i<max; ++i) {
        Document* doc = indexreader->document(i);
        if (doc) {
            docdump(doc);
        }
    }
    TermEnum* terms = indexreader->terms();
    while (terms->next()) {
        Term* t = terms->term();
        printf("%s: %s\n", t2a(t->field()).c_str(), t2a(t->text()).c_str());
        _CLDECDELETE(t);
    }
}
int
main(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
         indexdump(argv[i]);
    }
    return 0;
}
