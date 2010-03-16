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
#ifndef PNGENDANALYZER
#define PNGENDANALYZER

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>

class PngEndAnalyzerFactory;
class PngEndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const PngEndAnalyzerFactory* factory;
    time_t timeZoneOffset;
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "PngEndAnalyzer"; }
    signed char analyzeTime(Strigi::AnalysisResult& as, Strigi::InputStream* in);
    /*
       Internal function called to analyze text embedded in the png.
       Such text has a special format: 79 bytes of header, a \0 and content
       until the end of the stream.
     */
    signed char analyzeText(Strigi::AnalysisResult& as, Strigi::InputStream* in);
    signed char analyzeZText(Strigi::AnalysisResult& as, Strigi::InputStream* in);
    signed char addMetaData(const std::string& key, Strigi::AnalysisResult& as,
        Strigi::InputStream* in);
public:
    PngEndAnalyzer(const PngEndAnalyzerFactory* f);
};

class PngEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
friend class PngEndAnalyzer;
private:
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* colorDepthField;
    const Strigi::RegisteredField* colorModeField;
    const Strigi::RegisteredField* compressionField;
    const Strigi::RegisteredField* interlaceModeField;
    const Strigi::RegisteredField* lastModificationTimeField;
    const Strigi::RegisteredField* titleField;
    const Strigi::RegisteredField* authorField;
    const Strigi::RegisteredField* descriptionField;
    const Strigi::RegisteredField* copyrightField;
    const Strigi::RegisteredField* creationTimeField;
    const Strigi::RegisteredField* softwareField;
    const Strigi::RegisteredField* disclaimerField;
    const Strigi::RegisteredField* warningField;
    const Strigi::RegisteredField* sourceField;
    const Strigi::RegisteredField* commentField;
    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "PngEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new PngEndAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
