#include "xmlindexwriter.h"
using namespace std;
using namespace Strigi;

void
XmlIndexWriter::initWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end(f.fields().end());
    for (i = f.fields().begin(); i != end; ++i) {
        Tag* tag = static_cast<Tag*>(i->second->writerData());
        if (tag) {
            tag->refcount++;
            continue;
        }
        tag = new Tag();
        tag->refcount = 1;
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
    map<string, RegisteredField*>::const_iterator end(f.fields().end());
    for (i = f.fields().begin(); i != end; ++i) {
        Tag* tag = static_cast<Tag*>(i->second->writerData());
        if (tag->refcount-- == 1) {
            //fprintf(stderr, "free for %s\n", i->second->key().c_str());
            delete tag;
            i->second->setWriterData(0);
        }
    }
}
