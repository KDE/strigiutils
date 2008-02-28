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

#include "pdfendanalyzer.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "fieldtypes.h"
#include "textutils.h"
#include <sstream>
#include <cstring>
using namespace std;
using namespace Strigi;

void
PdfEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
    addField(typeField);
}

PdfEndAnalyzer::PdfEndAnalyzer(const PdfEndAnalyzerFactory* f) :factory(f) {
    parser.setStreamHandler(this);
    parser.setTextHandler(this);
}
StreamStatus
PdfEndAnalyzer::handle(InputStream* s) {
    ostringstream str;
    str << n++;
    char r = analysisresult->indexChild(str.str(), analysisresult->mTime(), s);
    // how do we set the error message in this case?
    return (r) ?Error :Ok;
}
StreamStatus
PdfEndAnalyzer::handle(const std::string& s) {
    analysisresult->addText(s.c_str(), s.length());
    return Ok;
}
bool
PdfEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return headersize > 7 && strncmp(header, "%PDF-1.", 7) == 0;
}
char
PdfEndAnalyzer::analyze(AnalysisResult& as, InputStream* in) {
    analysisresult = &as;
    n = 0;
    StreamStatus r = parser.parse(in);
    if (r != Eof) m_error.assign(parser.error());
    analysisresult->addValue(factory->typeField,
        "http://freedesktop.org/standards/xesam/1.0/core#TextDocument");
    return (r == Eof) ?0 :-1;
}
