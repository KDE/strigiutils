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
#include "rpmendanalyzer.h"
#include "rpminputstream.h"
#include "subinputstream.h"
#include "analysisresult.h"
using namespace Strigi;
using namespace jstreams;

void
RpmEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
RpmEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return RpmInputStream::checkHeader(header, headersize);
}
char
RpmEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    RpmInputStream rpm(in);
    InputStream *s = rpm.nextEntry();
    if (rpm.getStatus()) {
        fprintf(stderr, "error: %s\n", rpm.getError());
//        exit(1);
    }
    while (s) {
        idx.indexChild(rpm.getEntryInfo().filename, rpm.getEntryInfo().mtime,
            s);
        s = rpm.nextEntry();
    }
    if (rpm.getStatus() == Error) {
        error = rpm.getError();
        return -1;
    } else {
        error.resize(0);
    }
    return 0;
}

