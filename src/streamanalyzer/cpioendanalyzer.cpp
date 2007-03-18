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
#include "cpioendanalyzer.h"
#include "cpioinputstream.h"
#include "subinputstream.h"
#include "analysisresult.h"
using namespace Strigi;
using namespace jstreams;

void
CpioEndAnalyzerFactory::registerFields(FieldRegister& reg) {
}

bool
CpioEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return CpioInputStream::checkHeader(header, headersize);
}
char
CpioEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    CpioInputStream cpio(in);
    InputStream *s = cpio.nextEntry();
    if (cpio.getStatus()) {
        fprintf(stderr, "error: %s\n", cpio.getError());
//        exit(1);
    }
    while (s) {
        idx.indexChild(cpio.getEntryInfo().filename, cpio.getEntryInfo().mtime,
            *s);
        s = cpio.nextEntry();
    }
    if (cpio.getStatus() == Error) {
        fprintf(stderr, cpio.getError());
        error = cpio.getError();
        return -1;
    } else {
        error.resize(0);
    }
    return 0;
}

