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
#include "strigiconfig.h"
#include "oleinputstream.h"
#include "oleendanalyzer.h"
#include "subinputstream.h"
#include "analysisresult.h"
using namespace Strigi;

void
OleEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
OleEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return OleInputStream::checkHeader(header, headersize);
}
char
OleEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    OleInputStream ole(in);
    InputStream *s = ole.nextEntry();
    if (ole.status()) {
        fprintf(stderr, "error: %s\n", ole.error());
//        exit(1);
    }
    while (s) {
        idx.indexChild(ole.entryInfo().filename, ole.entryInfo().mtime,
            s);
        s = ole.nextEntry();
    }
    if (ole.status() == Error) {
        m_error = ole.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    return 0;
}

