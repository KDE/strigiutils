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
#ifndef ID3ENDANALYZER_H
#define ID3ENDANALYZER_H

#include <strigi/streamendanalyzer.h>
#include <strigi/fieldtypes.h>

// id3v2 according to http://www.id3.org/id3v2.4.0-structure.txt

class ID3EndAnalyzerFactory;
class ID3EndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const ID3EndAnalyzerFactory* factory;
public:
    ID3EndAnalyzer(const ID3EndAnalyzerFactory* f) :factory(f) {}
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "ID3EndAnalyzer"; }
};

class ID3EndAnalyzerFactory
        : public Strigi::StreamEndAnalyzerFactory {
friend class ID3EndAnalyzer;
private:
    const Strigi::RegisteredField* createdField;
    const Strigi::RegisteredField* subjectField;
    const Strigi::RegisteredField* titleField;
    const Strigi::RegisteredField* descriptionField;
    const Strigi::RegisteredField* commentField;
    const Strigi::RegisteredField* artistField;
    const Strigi::RegisteredField* albumField;
    const Strigi::RegisteredField* composerField;
    const Strigi::RegisteredField* performerField;
    const Strigi::RegisteredField* publisherField;
    const Strigi::RegisteredField* lyricistField;
    const Strigi::RegisteredField* languageField;
    const Strigi::RegisteredField* genreField;
    const Strigi::RegisteredField* copyrightField;
    const Strigi::RegisteredField* trackNumberField;
    const Strigi::RegisteredField* durationField;
    const Strigi::RegisteredField* typeField;
    const Strigi::RegisteredField* bitrateField;
    const Strigi::RegisteredField* samplerateField;
    const Strigi::RegisteredField* codecField;
    const Strigi::RegisteredField* channelsField;

    const char* name() const {
        return "ID3EndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new ID3EndAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
