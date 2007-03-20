/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "saxeventanalyzer.h"
#include "streamsaxanalyzer.h"
#include "analysisresult.h"
#include "textutils.h"
#include <libxml/parser.h>

using namespace jstreams;
using namespace Strigi;
using namespace std;


class SaxEventAnalyzer::Private {
public:
    enum FieldType { NONE, TEXT, TITLE };
    std::vector<StreamSaxAnalyzer*> sax;
    string fieldvalue;
    FieldType fieldtype;
    xmlParserCtxtPtr ctxt;
    xmlSAXHandler handler;
    AnalysisResult* idx;
    bool error;
    bool stop;
    int32_t chars;

    static void charactersSAXFunc(void* ctx, const xmlChar * ch, int len);
    static void errorSAXFunc(void* ctx, const char * msg, ...);
    static void startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes);

    Private(std::vector<StreamSaxAnalyzer*>& s) :sax(s) {
        ctxt = 0;
        memset(&handler, 0, sizeof(xmlSAXHandler));
        handler.characters = charactersSAXFunc;
        handler.error = errorSAXFunc;
        handler.startElementNs = startElementNsSAX2Func;
        fieldtype = TEXT;
    }
    ~Private() {
        reset();
        vector<StreamSaxAnalyzer*>::iterator s;
        for (s = sax.begin(); s != sax.end(); ++s) {
            delete *s;
        }
    }
    void reset() {
        if (ctxt) {
            xmlFreeParserCtxt(ctxt);
            ctxt = 0;
        }
        error = false;
        stop = false;
        chars = 0;
    }
    void init(AnalysisResult*i, const char* data, int32_t len) {
        reset();
        int initlen = (1024 > len) ?len :1024;
        idx = i;
        const char* name = 0;
        if (i) name = i->fileName().c_str();
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
SaxEventAnalyzer::Private::charactersSAXFunc(void* ctx, const xmlChar * ch,
        int len) {
    Private* p = (Private*)ctx;

    // skip whitespace
    const char* end = (const char*)ch+len;
    const char* c = (const char*)ch;
    while (c < end && isspace(*c)) c++;
    if (c == end) return;

    if (p->idx && p->fieldtype != NONE && checkUtf8((const char*)c, end-c)) {
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
SaxEventAnalyzer::Private::errorSAXFunc(void* ctx, const char* msg, ...) {
    Private* p = (Private*)ctx;
    p->stop = p->error = true;
    string e;

    va_list args;
    va_start(args, msg);
    e += string(" ")+va_arg(args,char*);
    va_end(args);
//    fprintf(stderr, "%s", e.c_str());
}
void
SaxEventAnalyzer::Private::startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes) {
    Private* p = (Private*)ctx;
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->startElement((const char*)localname, (const char*)prefix,
            (const char*)URI, nb_namespaces, (const char**)namespaces,
            nb_attributes, nb_defaulted, (const char**)attributes);
    }
}
SaxEventAnalyzer::SaxEventAnalyzer(std::vector<StreamSaxAnalyzer*>&s)
    :p(new Private(s)), ready(true) {
}
SaxEventAnalyzer::~SaxEventAnalyzer() {
    delete p;
}
void
SaxEventAnalyzer::startAnalysis(AnalysisResult* result) {
    ready = false;
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->startAnalysis(result);
    }
}
void
SaxEventAnalyzer::endAnalysis() {
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->endAnalysis();
    }
}
void
SaxEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (ready) return;
    p->push(data, length);
    bool more = false;
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        more = more || !(*i)->isReadyWithStream();
    }
    ready = !more;
}
bool
SaxEventAnalyzer::isReadyWithStream() {
    return ready;
}
