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

#ifndef STRIGI_HTMLSAXANALYZER
#define STRIGI_HTMLSAXANALYZER

#include <strigi/strigiconfig.h>
#include <strigi/streamsaxanalyzer.h>
#include <vector>

namespace Strigi {
class StreamSaxAnalyzer;
class HtmlSaxAnalyzer : public StreamSaxAnalyzer {
public:
    const char* name() const { return "HtmlSaxAnalyzer"; }
    void startAnalysis(AnalysisResult*);
    void endAnalysis(bool complete);
    void startElement(const char* localname, const char* prefix,
        const char* uri, int nb_namespaces, const char** namespaces,
        int nb_attributes,int nb_defaulted,const char** attributes);
    void endElement(const char* localname, const char* prefix,
        const char* uri);
    void characters(const char* data, uint32_t length);
    bool isReadyWithStream();
};
class HtmlSaxAnalyzerFactory : public StreamSaxAnalyzerFactory {
    const char* name() const {
        return "HtmlSaxAnalyzer";
    }
    void registerFields(Strigi::FieldRegister&) {
    }
    StreamSaxAnalyzer* newInstance() const {
        return new HtmlSaxAnalyzer();
    }
};
}
#endif
