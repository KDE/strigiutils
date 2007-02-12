#include "xmlindexwriter.h"
using namespace std;
using namespace jstreams;

void
XmlIndexWriter::initWriterData(const FieldRegister& f) {
    map<cnstr, RegisteredField*>::const_iterator i;
    map<cnstr, RegisteredField*>::const_iterator end = f.getFields().end();
    for (i = f.getFields().begin(); i != end; ++i) {
        Tag* tag = new Tag();
        const string s(i->first);
        const string& n = mapping.map(s);
        if (s == n) {
            tag->open = "  <value name='" + n + "'>";
            tag->close = "</value>\n";
        } else {
            tag->open = "  <" + n + ">";
            tag->close = "</" + n + ">\n";
        }
        i->second->setWriterData(tag);
    }
}
void
XmlIndexWriter::releaseWriterData(const FieldRegister& f) {
    map<cnstr, RegisteredField*>::const_iterator i;
    map<cnstr, RegisteredField*>::const_iterator end = f.getFields().end();
    for (i = f.getFields().begin(); i != end; ++i) {
        delete static_cast<Tag*>(i->second->getWriterData());
    }
}
