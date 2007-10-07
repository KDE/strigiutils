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
#include "zipendanalyzer.h"
#include <strigi/strigiconfig.h>
#include "zipinputstream.h"
#include "subinputstream.h"
#include "analysisresult.h"
#include "fieldtypes.h"
using namespace Strigi;

void
ZipEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    mimetypefield = reg.mimetypeField;
}

bool
ZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return ZipInputStream::checkHeader(header, headersize);
}
char
ZipEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    ZipInputStream zip(in);
    InputStream *s = zip.nextEntry();
    if (zip.status()) {
        fprintf(stderr, "error: %s\n", zip.error());
//        exit(1);
    }
    while (s) {
//        fprintf(stderr, "zip: %s\n", zip.entryInfo().filename.c_str());
        idx.indexChild(zip.entryInfo().filename, zip.entryInfo().mtime,
            s);
        s = zip.nextEntry();
    }
    if (zip.status() == Error) {
        m_error = zip.error();
        return -1;
    } else {
        idx.addValue(factory->mimetypefield, "application/zip");
        m_error.resize(0);
    }
    return 0;
}

