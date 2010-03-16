/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#ifndef FLACENDANALYZER_H
#define FLACENDANALYZER_H

#include <strigi/streamendanalyzer.h>
#include <strigi/fieldtypes.h>
#include <map>

// flac according to http://flac.sourceforge.net/format.html

class FlacEndAnalyzerFactory;
class FlacEndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const FlacEndAnalyzerFactory* factory;
public:
    FlacEndAnalyzer(const FlacEndAnalyzerFactory* f) :factory(f) {}
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "FlacEndAnalyzer"; }
};

class FlacEndAnalyzerFactory
        : public Strigi::StreamEndAnalyzerFactory {
friend class FlacEndAnalyzer;
private:
    std::map<std::string, const Strigi::RegisteredField*> fields;
    const Strigi::RegisteredField* artistField;
    const Strigi::RegisteredField* albumField;
    const Strigi::RegisteredField* composerField;
    const Strigi::RegisteredField* performerField;
    const Strigi::RegisteredField* publisherField;
    const Strigi::RegisteredField* descriptionField;
    const Strigi::RegisteredField* sampleRateField;
    const Strigi::RegisteredField* channelsField;
    const Strigi::RegisteredField* durationField;
    const Strigi::RegisteredField* bitsPerSampleField;
    const Strigi::RegisteredField* sampleCountField;
    const Strigi::RegisteredField* bitRateField;
    const Strigi::RegisteredField* codecField;
    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "FlacEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new FlacEndAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
