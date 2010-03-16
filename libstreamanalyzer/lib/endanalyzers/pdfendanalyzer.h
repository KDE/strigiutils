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
#ifndef PDFENDANALYZER
#define PDFENDANALYZER

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>
#include "../pdf/pdfparser.h"

class PdfEndAnalyzerFactory;
class PdfEndAnalyzer : public Strigi::StreamEndAnalyzer,
                       public PdfParser::StreamHandler,
                       public PdfParser::TextHandler {
private:
    PdfParser parser;
    const PdfEndAnalyzerFactory* const factory;
    Strigi::AnalysisResult* analysisresult;
    int n;

    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "PdfEndAnalyzer"; }
    Strigi::StreamStatus handle(Strigi::InputStream* s);
    Strigi::StreamStatus handle(const std::string& s);
public:
    PdfEndAnalyzer(const PdfEndAnalyzerFactory* f);
};

class PdfEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
friend class PdfEndAnalyzer;
private:
    const Strigi::RegisteredField* typeField;
public:
    const char* name() const {
        return "PdfEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new PdfEndAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
