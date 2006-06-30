#include "saxendanalyzer.h"
#include "streamindexer.h"
#include "inputstreamreader.h"
#include "indexwriter.h"
#include <libxml/parser.h>
using namespace jstreams;
using namespace std;

class SaxEndAnalyzer::Private {
public:
    xmlParserCtxtPtr ctxt;
    xmlSAXHandler handler;
    Indexable* idx;
    bool error;
    bool stop;
    string rootelement;
    int32_t chars;

    static void charactersSAXFunc(void* ctx, const xmlChar * ch, int len);
    static void errorSAXFunc(void* ctx, const char * msg, ...);
    static void startElementSAXFunc(void * ctx, const xmlChar * name, 
        const xmlChar ** atts);
    static void startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI, 
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes);

    Private() {
        ctxt = 0;
        memset(&handler, 0, sizeof(xmlSAXHandler));
        handler.characters = charactersSAXFunc;
        handler.error = errorSAXFunc;
        handler.startElement = startElementSAXFunc;
        handler.startElementNs = startElementNsSAX2Func;
    }
    ~Private() {
        reset();
    }
    void reset() {
        if (ctxt) {
            xmlFreeParserCtxt(ctxt);
            ctxt = 0;
        }
        error = false;
        stop = false;
        chars = 0;
        rootelement = "";
    }
    void init(Indexable*i, const char* data, int32_t len) {
        reset();
        int initlen = (1024 > len) ?len :1024;
        idx = i;
        const char* name = 0;
        if (i) name = i->getName().c_str();
        xmlKeepBlanksDefault(0);
        ctxt = xmlCreatePushParserCtxt(&handler, this, data, initlen, name);
        if (ctxt == 0) {
            error = true;
            stop = true;
        } else {
//            ctxt->sax2 = 1;
            // we need to call push once to do validation
            push(data+initlen, len-initlen);
        }
    }
    void push(const char* data, int32_t len) {
        xmlParseChunk(ctxt, data, len, 0);
    }
    void finish() {
        xmlParseChunk(ctxt, 0, 0, 1);
    }
};
void
SaxEndAnalyzer::Private::charactersSAXFunc(void* ctx, const xmlChar * ch,
        int len) {
    Private* p = (Private*)ctx;

    // skip whitespace
    const char* end = (const char*)ch+len;
    const char* c = (const char*)ch;
    while (c < end && isspace(*c)) c++;
    if (c == end) return;

    if (p->idx) {
        p->idx->addText((const char*)c, end-c);
    }
    p->chars += end-c;
    if (p->chars > 1000000) {
        p->stop = true;
    }
}
void
SaxEndAnalyzer::Private::errorSAXFunc(void* ctx, const char* msg, ...) {
    Private* p = (Private*)ctx;
    p->stop = p->error = true;
}
void
SaxEndAnalyzer::Private::startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI, 
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes) {
    Private* p = (Private*)ctx;
    if(URI && p->rootelement.size() == 0) {
        p->rootelement = (const char*)URI;
    }
}
void
SaxEndAnalyzer::Private::startElementSAXFunc(void* ctx, const xmlChar* name, 
        const xmlChar** atts) {
    Private* p = (Private*)ctx;
    if(name && p->rootelement.size() == 0) {
        p->rootelement = (const char*)name;
    }
}
SaxEndAnalyzer::SaxEndAnalyzer() {
    p = new Private();
}
SaxEndAnalyzer::~SaxEndAnalyzer() {
    delete p;
}

bool
SaxEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    p->init(0, header, headersize);
    return !p->error;
}

char
SaxEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, Indexable* i) {

    const char* b;
    int32_t nread = in->read(b, 4, 0);
    if (nread >= 4) {
        p->init(i, b, nread);
        nread = in->read(b, 1, 0);
    }
    while (nread > 0 && !p->stop) {
        p->push(b, nread);
        nread = in->read(b, 1, 0);
    }
    p->finish();
    if (p->ctxt->encoding) {
        i->setField("encoding", (const char*)p->ctxt->encoding);
    }
    i->setMimeType("text/xml");
    i->setField("root", p->rootelement);
    if (nread != Eof) {
        error = in->getError();
        return -1;
    }
    return 0;
}
