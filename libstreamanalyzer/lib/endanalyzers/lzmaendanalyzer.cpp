/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
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
#include "lzmaendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/lzmainputstream.h>
#include "tarendanalyzer.h"
#include <strigi/tarinputstream.h>
#include <strigi/streamanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

using namespace std;
using namespace Strigi;

void
LzmaEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
    addField(typeField);
}

bool
LzmaEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    bool v = LZMAInputStream::checkHeader(header, headersize);
    return v;
}
signed char
LzmaEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    LZMAInputStream stream(in);
    // since this is lzma file, its likely that it contains a tar file
    const char* start = 0;
    int32_t nread = stream.read(start, 1024, 0);
    if (nread < -1) {
        fprintf(stderr, "Error reading lzma: %s\n", stream.error());
        return -2;
    }
    idx.addValue(factory->typeField,
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive");
    stream.reset(0);
    if (TarInputStream::checkHeader(start, nread)) {
        return TarEndAnalyzer::staticAnalyze(idx, &stream);
    } else {
        std::string name = idx.fileName();
        string::size_type len = name.length();
        if (len > 5 && name.substr(len-5)==".lzma") {
            name = name.substr(0, len-5);
        }
        return idx.indexChild(name, idx.mTime(), &stream);
    }
}
