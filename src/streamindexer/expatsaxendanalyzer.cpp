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
    bool stop, error;

    static void charactersSAXFunc(void* ctx, const char * ch, int len);
    static void errorSAXFunc(void* ctx, const char * msg, ...);
    static void startElementSAXFunc(void * ctx, const char * name, 
        const char ** atts);
    static void endElementSAXFunc(void * ctx, const char * name);

    Private() {
        idx = 0;
        chars = 0;
        stop = false;
        error = false;
        parser = XML_ParserCreate(NULL);
        XML_SetElementHandler(this->parser,
            startElementSAXFunc, endElementSAXFunc);
        XML_SetCharacterDataHandler(this->parser, charactersSAXFunc);
    }
    ~Private() {
    }
    void reset() {
    }
    void init(Indexable*i, const char* data, int32_t len) {
    }
    void push(const char* data, int32_t len) {
    }
    void finish() {
    }
};
void
SaxEndAnalyzer::Private::charactersSAXFunc(void* ctx, const char * ch,
        int len) {
    Private* p = (Private*)ctx;

    // skip whitespace
    const char* end = (const char*)ch+len;
    const char* c = (const char*)ch;
    while (c < end && isspace(*c)) c++;
    if (c == end) return;

    if (p->idx && p->fieldtype != NONE) {
        if (p->fieldtype == TEXT) {
            p->idx->addText((const char*)c, end-c);
        } else {
            p->fieldvalue += string((const char*)c, end-c);
        }
    }
    p->chars += end-c;
    if (p->chars > 1000000) {
        p->stop = true;
    }
}
#include <iostream>
void
SaxEndAnalyzer::Private::errorSAXFunc(void* ctx, const char* msg, ...) {
    Private* p = (Private*)ctx;
    p->stop = p->error = true;
    string e;

    printf("%s", e.c_str());
}
void
SaxEndAnalyzer::Private::startElementSAXFunc(void* ctx, const char* name, 
        const char** atts) {
    Private* p = (Private*)ctx;
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
    i->setMimeType("text/xml");
//    i->setField("root", p->rootelement);
    if (nread != Eof) {
        error = in->getError();
        return -1;
    }
    return 0;
}
