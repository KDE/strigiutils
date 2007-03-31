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
#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "subinputstream.h"
#include "analysisresult.h"
using namespace Strigi;

void
TarEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
TarEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return TarInputStream::checkHeader(header, headersize);
}
char
TarEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    return staticAnalyze(idx, in);
}
char
TarEndAnalyzer::staticAnalyze(AnalysisResult& idx,
        InputStream* in) {
    if(!in)
        return -1;

    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
    while (s) {
        idx.indexChild(tar.entryInfo().filename, tar.entryInfo().mtime,
            s);
        s = tar.nextEntry();
    }
    if (tar.status() == Error) {
        return -1;
//        printf("Error: %s\n", tar.error());
    } else {
//        printf("finished ok\n");
    }
    return 0;
}

