/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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
#ifndef ODFENDANALYZER
#define ODFENDANALYZER

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>
#include "../helperanalyzers/odfmetahelperanalyzer.h"
#include "../helperanalyzers/odfcontenthelperanalyzer.h"


class OdfEndAnalyzerFactory;

class OdfEndAnalyzer : public Strigi::StreamEndAnalyzer {
public:
    const OdfEndAnalyzerFactory* const factory;

    Strigi::OdfMetaHelperAnalyzer metaHelper;
    Strigi::OdfContentHelperAnalyzer contentHelper;
    
    OdfEndAnalyzer(const OdfEndAnalyzerFactory* f) :factory(f){};
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "OdfEndAnalyzer"; }

};

class OdfEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
public:
  
    const char* name() const {
        return "OdfEndAnalyzer";
    }
    const Strigi::RegisteredField* typeField;
    
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new OdfEndAnalyzer(this);
    }
    bool analyzesSubStreams() const { return true; }
    void registerFields(Strigi::FieldRegister&);
};




#endif
