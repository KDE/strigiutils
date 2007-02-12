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
#ifndef SAXENDANALYZER
#define SAXENDANALYZER

#include "streamendanalyzer.h"

class SaxEndAnalyzerFactory;
class SaxEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    class Private;
private:
    Private* p;
public:
    SaxEndAnalyzer(const SaxEndAnalyzerFactory* f);
    ~SaxEndAnalyzer();
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(jstreams::Indexable& idx, jstreams::InputStream* in);
    const char* getName() const { return "SaxEndAnalyzer"; }
};

class SaxEndAnalyzerFactory : public jstreams::StreamEndAnalyzerFactory {
friend class SaxEndAnalyzer::Private;
public:
    static const cnstr titleFieldName;
private:
    const jstreams::RegisteredField* titleField;
    const char* getName() const {
        return "SaxEndAnalyzer";
    }
    jstreams::StreamEndAnalyzer* newInstance() const {
        return new SaxEndAnalyzer(this);
    }
    void registerFields(jstreams::FieldRegister&);
};

#endif
