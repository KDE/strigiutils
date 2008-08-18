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
#include "arendanalyzer.h"
#include <strigi/strigiconfig.h>
#include "arinputstream.h"
#include "analysisresult.h"
#include "fieldtypes.h"
#include "subinputstream.h"
using namespace Strigi;

void
ArEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
}

bool
ArEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return ArInputStream::checkHeader(header, headersize);
}
signed char
ArEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    char result = staticAnalyze(idx, in);
    idx.addValue(factory->typeField, "http://freedesktop.org/standards/xesam/1.0/core#Archive");
    return  result;
}
signed char
ArEndAnalyzer::staticAnalyze(AnalysisResult& idx,
        InputStream* in) {
    if(!in)
        return -1;

    ArInputStream ar(in);
    InputStream *s = ar.nextEntry();
    while (s) {
        idx.indexChild(ar.entryInfo().filename, ar.entryInfo().mtime, s);
        s = ar.nextEntry();
    }
    if (ar.status() == Error) {
        return -1;
//        printf("Error: %s\n", ar.error());
    } else {
//        printf("finished ok\n");
    }
    return 0;
}

