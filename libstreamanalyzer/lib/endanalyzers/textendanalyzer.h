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
#ifndef TEXTENDANALYZER
#define TEXTENDANALYZER

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>

class TextEndAnalyzer : public Strigi::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "TextEndAnalyzer"; }
};

class TextEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
public:
    const char* name() const {
        return "TextEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new TextEndAnalyzer();
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
