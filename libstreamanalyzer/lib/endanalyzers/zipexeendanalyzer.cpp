/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2008 Jos van den Oever <jos@vandenoever.info>
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
#include "zipexeendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/zipinputstream.h>
#include <strigi/subinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/fieldtypes.h>
using namespace Strigi;

void
ZipExeEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    mimetypefield = reg.mimetypeField;
    typeField = reg.typeField;
}

bool
ZipExeEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char magic[] = {0x4d, 0x5a, 0x90, 0x00};
    if (headersize < 4) return false;
    bool ok = std::memcmp(header, magic, 4) == 0;
    return ok;
}
signed char
ZipExeEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    // read 64k
    const char* c;
    int32_t max = 131072;
    int32_t nread = in->read(c, max, max);
    if (nread <= 0 || in->status() == Error) {
        m_error = in->error();
        return -1;
    }
    in->reset(0);
    // find a zip signature
    int32_t offset = 0;
    for (int32_t i = 4; i<nread-4; ++i) {
        if (zipanalyzer.checkHeader(c+i-offset, nread-i)) {
            char r;
            if (in->reset(i) == i) {
                // put the stream in a substream so it can be reset to the start
                SubInputStream sub(in, 0);
                r = zipanalyzer.analyze(idx, &sub);
                if (r == 0) {
                    return 0;
                }
            } else {
                // zip analyzer has read too much, we cannot recover
                return -1;
            }
            offset = (uint32_t)in->reset(i);
            if (offset != i) {
                return r;
            }
            max = 131072-offset;
            nread = in->read(c, max, max);
            if (nread <= 0 || in->status() == Error) {
                return -1;
            }
            offset = (uint32_t)in->reset(i);
        }
    }
    return -1;
}

