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
#include <strigi/arinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/subinputstream.h>
#include <strigi/analyzerconfiguration.h>
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
    idx.addValue(factory->typeField,
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive");
    return  result;
}
signed char
ArEndAnalyzer::staticAnalyze(AnalysisResult& idx,
        InputStream* in) {
    if(!in)
        return -1;

    ArInputStream ar(in);
    // if the first two files are called 'debian-binary' and 'control.tar.gz'
    // those are analyzed regardless, since they signal that this file is a 
    // debian archive
    InputStream *s = ar.nextEntry();
    if (s && ar.entryInfo().filename.compare("debian-binary") == 0) {
        idx.indexChild(ar.entryInfo().filename, ar.entryInfo().mtime, s);
        s = ar.nextEntry();
    }
    if (s && ar.entryInfo().filename.compare("control.tar.gz") == 0) {
        idx.indexChild(ar.entryInfo().filename, ar.entryInfo().mtime, s);
        s = ar.nextEntry();
    }
    if (idx.config().indexArchiveContents()) {
        while (s) {
            // check if we're done
            int64_t max = idx.config().maximalStreamReadLength(idx);
            if (max != -1 && in->position() > max) {
                return 0;
            }
            // check if the analysis has been aborted
            if (!idx.config().indexMore()) {
                return 0;
            }
            idx.indexChild(ar.entryInfo().filename, ar.entryInfo().mtime, s);
            s = ar.nextEntry();
        }
    }
    if (ar.status() == Error) {
        return -1;
//        printf("Error: %s\n", ar.error());
    } else {
//        printf("finished ok\n");
    }
    return 0;
}

