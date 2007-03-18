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
#include "textendanalyzer.h"
#include "streamanalyzer.h"
#include "inputstreamreader.h"
#include "analysisresult.h"
#include "textutils.h"
using namespace jstreams;
using namespace Strigi;

void
TextEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
TextEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return checkUtf8(header, headersize);
/*    const char* end = header + headersize;
    const char* p = header;
    // check if the text is valid UTF-8
    char nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                return false;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            nb = 3;
        } else if (c <= 8) {
            return false;
        }
        p++;
    }
    return true;*/
}

char
TextEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    if (idx.encoding().length()) {
//        fprintf(stderr, "%s\n", idx.encoding().c_str());
    }
    // pass a piece of text to the indexer. it's up to the indexer to break
    // it down into words
    const char* b;
    // store and index the first 20k of a text file
    int32_t nread = in->read(b, 20*1024, 0);
    if (nread > 0 && checkUtf8(b, nread)) {
        idx.addText(b, nread);
    }
    if (in->getStatus() == Error) {
        error = in->getError();
        return -1;
    }
//    InputStreamReader reader(in);
//    i->addStream("content", &reader);
    return 0;
}
