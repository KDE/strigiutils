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
#include "gzipendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/gzipinputstream.h>
#include "tarendanalyzer.h"
#include <strigi/tarinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

using namespace Strigi;
using namespace std;

void
GZipEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
    addField(typeField);
}

bool
GZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return headersize > 2 && (unsigned char)header[0] == 0x1f
        && (unsigned char)header[1] == 0x8b;
}
signed char
GZipEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    GZipInputStream stream(in);
    // since this is gzip file, its likely that it contains a tar file
    const char* start = 0;
    int32_t nread = stream.read(start, 1024, 0);
    if (nread < -1) {
        printf("Error reading gzip: %s\n", stream.error());
        return -2;
    }

    idx.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive");

    stream.reset(0);
    if (TarInputStream::checkHeader(start, nread)) {
        return TarEndAnalyzer::staticAnalyze(idx, &stream);
    } else {
        std::string file = idx.fileName();
        size_t len = file.length();
        if (len > 3 && file.substr(len-3) == ".gz") {
            file = file.substr(0, len-3);
        }
        return idx.indexChild(file, idx.mTime(), &stream);
    }
}
