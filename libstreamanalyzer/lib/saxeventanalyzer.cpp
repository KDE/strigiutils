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
#include <strigi/streamsaxanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include <libxml/SAX2.h>
#include <iostream>
#include <cassert>
#include <cstring>

#ifndef LIBXML_THREAD_ENABLED
#error Strigi needs a thread safe version of libxml2
#endif

using namespace Strigi;
using namespace std;

class xmlCleaner {
public:
    ~xmlCleaner() {
         xmlCleanupParser();
    }
};
static xmlCleaner xmlcleaner;

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
    static void endElementNsSAX2Func(void *ctx,
        const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

    Private(std::vector<StreamSaxAnalyzer*>& s) :sax(s) {
        ctxt = 0;
        memset(&handler, 0, sizeof(xmlSAXHandler));
        handler.initialized = XML_SAX2_MAGIC;
        handler.characters = charactersSAXFunc;
        handler.error = errorSAXFunc;
        handler.startElementNs = startElementNsSAX2Func;
        handler.endElementNs = endElementNsSAX2Func;
    }
    ~Private() {
        vector<StreamSaxAnalyzer*>::iterator s;
        for (s = sax.begin(); s != sax.end(); ++s) {
            delete *s;
        }
        if (ctxt) {
            xmlFreeParserCtxt(ctxt);
        }
    }
    void init(const char* data, int32_t len) {
        error = false;
        int initlen = (512 > len) ?len :512;
        const char* name = result->fileName().c_str();
        xmlKeepBlanksDefault(0);
        if (ctxt) {
            xmlCtxtResetPush(ctxt, data, initlen, name, 0);
        } else {
            ctxt = xmlCreatePushParserCtxt(&handler, this, data, initlen, name);
        }
        if (ctxt == 0) {
            error = true;
        } else if (len > initlen) {
            push(data + initlen, len - initlen);
        }
    }
    void push(const char* data, int32_t len) {
        if (xmlParseChunk(ctxt, data, len, 0)) {
            error = true;
        }
    }
    void finish() {
        if (xmlParseChunk(ctxt, 0, 0, 1)) {
            error = true;
        }
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
void
SaxEventAnalyzer::Private::endElementNsSAX2Func(void *ctx,
        const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {
    Private *p = (Private *) ctx;
    vector<StreamSaxAnalyzer *>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->endElement((const char *) localname, (const char *) prefix,
                         (const char *) URI);
    }
}
SaxEventAnalyzer::SaxEventAnalyzer(std::vector<StreamSaxAnalyzer*>& s)
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

    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->startAnalysis(p->result);
    }
}
void
SaxEventAnalyzer::endAnalysis(bool complete) {
    vector<StreamSaxAnalyzer*>::iterator i;
    for (i = p->sax.begin(); i != p->sax.end(); ++i) {
        (*i)->endAnalysis(complete);
    }
}
void
SaxEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (ready) return;
    // push the data into the parser
    if (!initialized) {
        p->init(data, length);
        initialized = true;
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
