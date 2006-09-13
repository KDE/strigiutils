/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "jstreamsconfig.h"
#include "saxendanalyzer.h"
#include "streamindexer.h"
#include "inputstreamreader.h"
#include "indexwriter.h"
#include <expat.h>
using namespace jstreams;
using namespace std;

class SaxEndAnalyzer::Private {
public:
    enum FieldType { NONE, TEXT, TITLE };
    string fieldvalue;
    FieldType fieldtype;
    XML_Parser parser;
    Indexable* idx;
    int chars;
    int depth;
    bool stop;
    bool error;
    bool wellformed;
    bool html;
    string errorstring;

    static void charactersSAXFunc(void* ctx, const char * ch, int len);
    static void startElementSAXFunc(void * ctx, const char * name,
        const char ** atts);
    static void endElementSAXFunc(void * ctx, const char * name);

    Private() {
        idx = 0;
        parser = XML_ParserCreate(NULL);
    }
    ~Private() {
        XML_ParserFree(parser);
    }
    void reset() {
        XML_ParserReset(parser, 0);
        XML_SetElementHandler(this->parser,
            startElementSAXFunc, endElementSAXFunc);
        XML_SetCharacterDataHandler(this->parser, charactersSAXFunc);
        XML_SetUserData(parser, this);
        error = stop = false;
        chars = 0;
        stop = false;
        error = false;
        html = false;
        wellformed = true;
        depth = 0;
    }
    void init(Indexable*i, const char* data, int32_t len) {
        idx = i;
        reset();
        push(data, len);
    }
    void push(const char* data, int32_t len) {
        if (XML_Parse(parser, data, len, false) == 0) {
            XML_Error e = XML_GetErrorCode(parser);
            // handle the error unless it is a tag mismatch in html
            if (!(html && e == XML_ERROR_TAG_MISMATCH)) {
                errorstring = XML_ErrorString(e);
                error = stop = true;
            }
            wellformed = false;
        }
    }
    void finish() {
        XML_Parse(parser, 0, 0, false);
    }
};
void
SaxEndAnalyzer::Private::charactersSAXFunc(void* ctx, const char * ch,
        int len) {
    if (ctx == 0) return;
    Private* p = (Private*)ctx;
    if (p->idx == 0) return;

    // skip whitespace
    const char* end = ch+len;
    while (ch < end && isspace(*ch)) ch++;
    if (ch == end) {
        p->idx->addText(" ", 1);
        return;
    }
    len = end-ch;
    if (p->fieldtype != NONE) {
        if (p->fieldtype == TEXT) {
            p->idx->addText(ch, len);
        } else {
            p->fieldvalue += string(ch, len);
        }
    }
    p->chars += len;
    if (p->chars > 1000000) {
        p->stop = true;
    }
}
void
SaxEndAnalyzer::Private::startElementSAXFunc(void* ctx, const char* name,
        const char** atts) {
    Private* p = (Private*)ctx;
    if (p->depth++ == 0 && strcasecmp((const char*)name, "html") == 0) {
        p->html = true;
    }
    if (strcasecmp((const char*)name, "title") == 0) {
        p->fieldtype = TITLE;
        p->fieldvalue = "";
    }
}
void
SaxEndAnalyzer::Private::endElementSAXFunc(void* ctx, const char* name) {
    Private* p = (Private*)ctx;
    if (p->idx && p->fieldtype == TITLE && p->fieldvalue.size()) {
        p->idx->setField("title", p->fieldvalue);
        p->fieldvalue = "";
    }
    if (p->depth) p->depth--;
    p->fieldtype = TEXT;
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
/*    if (p->ctxt->encoding) {
        i->setField("encoding", (const char*)p->ctxt->encoding);
    }*/
    if (p->html) {
        i->setMimeType("text/html");
    } else if (p->wellformed) {
        i->setMimeType("text/xml");
    }
//    i->setField("root", p->rootelement);
    if (in->getStatus() != Eof) {
        error = in->getError();
        return -1;
    }
    return 0;
}
