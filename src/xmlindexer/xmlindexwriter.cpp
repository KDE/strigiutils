#include "xmlindexwriter.h"
using namespace std;
using namespace Strigi;

void
XmlIndexWriter::initWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        Tag* tag = new Tag();
        const string s(i->first);
        const string& n = mapping.map(s);
        if (s == n) {
            tag->open = "  <value name='" + n + "'>";
            tag->close = "</value>\n";
        } else {
            tag->open = "  <" + n + '>';
            tag->close = "</" + n + ">\n";
        }
        i->second->setWriterData(tag);
    }
}
void
XmlIndexWriter::releaseWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        delete static_cast<Tag*>(i->second->writerData());
    }
}
