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
#include <libxml/SAX2.h>

using namespace jstreams;
using namespace Strigi;
using namespace std;

class SaxEventAnalyzer::Private {
public:
    std::vector<StreamSaxAnalyzer*> sax;
    xmlParserCtxtPtr ctxt;
    xmlSAXHandler handler;
    AnalysisResult* result;
    bool error;

    static void charactersSAXFunc(void* ctx, const xmlChar * ch, int len);
    static void errorSAXFunc(void* ctx, const char * msg, ...);
    static void startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes);

    Private(std::vector<StreamSaxAnalyzer*>& s) :sax(s) {
        ctxt = 0;
        memset(&handler, 0, sizeof(xmlSAXHandler));
        handler.initialized = XML_SAX2_MAGIC;
        handler.characters = charactersSAXFunc;
        handler.error = errorSAXFunc;
        handler.startElementNs = startElementNsSAX2Func;
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
    }
    void init(const char* data, int32_t len) {
        reset();
        int initlen = (1024 > len) ?len :1024;
        const char* name = result->fileName().c_str();
        xmlKeepBlanksDefault(0);
        ctxt = xmlCreatePushParserCtxt(&handler, this, data, initlen, name);
        if (ctxt == 0) {
            error = true;
        } else {
            push(data + initlen, len - initlen);
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
SaxEventAnalyzer::Private::charactersSAXFunc(void* ctx, const xmlChar* ch,
        int len) {
    Private* p = (Private*)ctx;
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->characters((const char*)ch, len);
    }
}
#include <iostream>
void
SaxEventAnalyzer::Private::errorSAXFunc(void* ctx, const char* msg, ...) {
    Private* p = (Private*)ctx;
    p->error = true;
    string e;

    va_list args;
    va_start(args, msg);
    e += string(" ")+va_arg(args,char*);
    va_end(args);
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
    p->result = result;
    ready = false;
    initialized = false;
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
    // push the data into the parser
    if (!initialized) {
        p->init(data, length);
        initialized = true;
        if (!p->error) {
            vector<StreamSaxAnalyzer*>::iterator i;
            for (i = p->sax.begin(); i != p->sax.end(); ++i) {
                (*i)->startAnalysis(p->result);
            }
        }
    } else {
        p->push(data, length);
    }

    // check if we are done
    bool more = false;
    if (!p->error) {
        vector<StreamSaxAnalyzer*>::iterator i;
        for (i = p->sax.begin(); i != p->sax.end(); ++i) {
            more = more || !(*i)->isReadyWithStream();
        }
    }
    ready = !more;
}
bool
SaxEventAnalyzer::isReadyWithStream() {
    return ready;
}
