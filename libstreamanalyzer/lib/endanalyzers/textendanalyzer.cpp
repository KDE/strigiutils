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
#include "textendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/streamanalyzer.h>
#include <strigi/inputstreamreader.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include <iostream>
using namespace Strigi;
using namespace std;

void
TextEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
TextEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    char nb;
    const char* last = checkUtf8(header, headersize, nb);
    return last == 0 || nb != 0;
}

signed char
TextEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    // pass a piece of text to the indexer. it's up to the indexer to break
    // it down into words
    const char* b;
    // store and index the first 20k of a text file
    int32_t nread = in->read(b, 20*1024, 0);
    signed char retval = -1;
    if (nread > 0) {
        char nb;
        const char* last = checkUtf8(b, nread, nb);
        if (last == 0) {
            retval = 0;
            idx.addText(b, nread);
        } else if (nb) {
            retval = 0;
            idx.addText(b, (uint32_t)(last-b));
        }
    }
    if (in->status() == Error) {
        m_error = in->error();
        return -1;
    }
    return retval;
}
