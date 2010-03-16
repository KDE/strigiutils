/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * 		 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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
#include "saxhelperanalyzer.h"
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include <iostream>
#include <cassert>
#include <cstring>

using namespace Strigi;
using namespace std;

SaxHelperAnalyzer::SaxHelperAnalyzer() {
    ctxt = 0;
    memset(&handler, 0, sizeof(xmlSAXHandler));
    handler.initialized = XML_SAX2_MAGIC;
    handler.characters = charactersSAXFunc;
    handler.error = errorSAXFunc;
    handler.startElementNs = startElementNsSAX2Func;
    handler.endElementNs = endElementNsSAX2Func;
}

SaxHelperAnalyzer::~SaxHelperAnalyzer() {
    if (ctxt)
	xmlFreeParserCtxt(ctxt);
}

void
SaxHelperAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    const char *buf;
    size_t nread;
    
    if (!in) return;

    initialized = false;
    startAnalysis(idx);
    do {
      nread = in->read(buf,10240,10240);
      if (nread > 0)
	  handleData(buf, nread);
    } while (nread == 10240) ;
    finish();
    endAnalysis(true);
}

void SaxHelperAnalyzer::init(const char* data, int32_t len) {
        error = false;
        int initlen = (512 > len) ?len :512;
        xmlKeepBlanksDefault(0);
        if (ctxt) {
            xmlCtxtResetPush(ctxt, data, initlen, /*result->fileName().c_str()*/NULL, 0);
        } else {
            ctxt = xmlCreatePushParserCtxt(&handler, this, data, initlen, /*result->fileName().c_str()*/ NULL);
        }
        if (ctxt == 0) {
            error = true;
        } else if (len > initlen) {
            push(data + initlen, len - initlen);
        }
    }

void SaxHelperAnalyzer::push(const char* data, int32_t len) {
    if (xmlParseChunk(ctxt, data, len, 0)) {
	error = true;
    }
}

void SaxHelperAnalyzer::finish() {
    if (xmlParseChunk(ctxt, 0, 0, 1)) {
	error = true;
    }
}

void
SaxHelperAnalyzer::handleData(const char* data, uint32_t length) {
    // push the data into the parser
    if (!initialized) {
        init(data, length);
        initialized = true;
    } else {
        push(data, length);
    }
}
