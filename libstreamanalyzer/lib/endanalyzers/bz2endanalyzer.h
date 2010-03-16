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
#ifndef BZ2ENDANALYZER
#define BZ2ENDANALYZER

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>

class Bz2EndAnalyzerFactory;
class Bz2EndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const Bz2EndAnalyzerFactory* factory;
public:
    Bz2EndAnalyzer(const Bz2EndAnalyzerFactory* f)
        :factory(f) {}

    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "Bz2EndAnalyzer"; }
};

class Bz2EndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
friend class Bz2EndAnalyzer;
private:
    const Strigi::RegisteredField* typeField;
public:
    const char* name() const {
        return "Bz2EndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new Bz2EndAnalyzer(this);
    }
    bool analyzesSubStreams() const { return true; }
    void registerFields(Strigi::FieldRegister&);
};

#endif
