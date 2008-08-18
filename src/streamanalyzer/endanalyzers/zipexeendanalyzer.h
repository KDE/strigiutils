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
#ifndef ZIPEXEENDANALYZER
#define ZIPEXEENDANALYZER

#include "zipendanalyzer.h"

class ZipExeEndAnalyzerFactory;
class ZipExeEndAnalyzer : public Strigi::StreamEndAnalyzer {
public:
    const ZipExeEndAnalyzerFactory* const factory;
    ZipEndAnalyzer zipanalyzer;

    ZipExeEndAnalyzer(const ZipExeEndAnalyzerFactory* f) :factory(f),
        zipanalyzer(0) {}
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "ZipExeEndAnalyzer"; }
};

class ZipExeEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
public:
    const char* name() const {
        return "ZipExeEndAnalyzer";
    }
    const Strigi::RegisteredField* mimetypefield;
    const Strigi::RegisteredField* typeField;

    Strigi::StreamEndAnalyzer* newInstance() const {
        return new ZipExeEndAnalyzer(this);
    }
    bool analyzesSubStreams() const { return true; }
    void registerFields(Strigi::FieldRegister&);
};


#endif
