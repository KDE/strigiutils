/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Vincent Ricard <magic@magicninja.org>
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
#ifndef STRIGI_XBMTHROUGHANALYZER
#define STRIGI_XBMTHROUGHANALYZER

#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

#include <string>

namespace Strigi {
    class RegisteredField;
}
class XbmThroughAnalyzerFactory;

class STRIGI_PLUGIN_API XbmThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const XbmThroughAnalyzerFactory* factory;
public:
    XbmThroughAnalyzer(const XbmThroughAnalyzerFactory* f) :factory(f) {}
    ~XbmThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "XbmThroughAnalyzer"; }
};

class XbmThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class XbmThroughAnalyzer;
private:
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* xHotField;
    const Strigi::RegisteredField* yHotField;

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "XbmThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new XbmThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
