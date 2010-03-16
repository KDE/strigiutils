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

#ifndef OGGTHROUGHANALYZER_H
#define OGGTHROUGHANALYZER_H

#include <strigi/streamthroughanalyzer.h>
#include <strigi/fieldtypes.h>
#include <map>

// ogg according to http://tools.ietf.org/html/rfc3533
// and http://xiph.org/ogg/doc/rfc3534.txt
// and http://xiph.org/vorbis/doc/v-comment.html

class OggThroughAnalyzerFactory;
class OggThroughAnalyzer : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* indexable;
    const OggThroughAnalyzerFactory* factory;
public:
    OggThroughAnalyzer(const OggThroughAnalyzerFactory* f) :factory(f) {}
    ~OggThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult*);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "OggThroughAnalyzer"; }
};

class OggThroughAnalyzerFactory
        : public Strigi::StreamThroughAnalyzerFactory {
friend class OggThroughAnalyzer;
private:
    std::map<std::string, const Strigi::RegisteredField*> fields;
    const Strigi::RegisteredField* artistField;
    const Strigi::RegisteredField* albumField;
    const Strigi::RegisteredField* composerField;
    const Strigi::RegisteredField* performerField;
    const char* name() const {
        return "OggThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new OggThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
