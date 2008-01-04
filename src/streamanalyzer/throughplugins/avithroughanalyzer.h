/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#ifndef STRIGI_AVITHROUGHANALYZER
#define STRIGI_AVITHROUGHANALYZER

#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"

#include <string>

namespace Strigi {
    class RegisteredField;
}
class AviThroughAnalyzerFactory;

class STRIGI_PLUGIN_API AviThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const AviThroughAnalyzerFactory* factory;

    bool done_avih;
    uint32_t avih_microsecperframe;
    uint32_t avih_maxbytespersec;
    uint32_t avih_reserved1;
    uint32_t avih_flags;
    uint32_t avih_totalframes;
    uint32_t avih_initialframes;
    uint32_t avih_streams;
    uint32_t avih_buffersize;
    uint32_t avih_width;
    uint32_t avih_height;
    uint32_t avih_scale;
    uint32_t avih_rate;
    uint32_t avih_start;
    uint32_t avih_length;

    char handler_vids[5];   // leave room for trailing \0
    char handler_auds[5];
    uint16_t handler_audio; // the ID of the audio codec
    bool done_audio;
    
    bool wantstrf;

    bool read_avi(Strigi::InputStream* in);
    bool read_strf(Strigi::InputStream* in, uint32_t blocksize);
    bool read_strh(Strigi::InputStream* in, uint32_t blocksize);
    bool read_strl(Strigi::InputStream* in);
    bool read_avih(Strigi::InputStream* in);
    bool read_list(Strigi::InputStream* in);
    const char* resolve_audio(uint16_t id);
    
public:
    AviThroughAnalyzer(const AviThroughAnalyzerFactory* f) :factory(f) {}
    ~AviThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "AviThroughAnalyzer"; }
};

class AviThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class AviThroughAnalyzer;
private:
    static const std::string lengthFieldName;
    static const std::string resolutionHeightFieldName;
    static const std::string resolutionWidthFieldName;
    static const std::string frameRateFieldName;
    static const std::string videoCodecFieldName;
    static const std::string audioCodecFieldName;
    const Strigi::RegisteredField* lengthField;
    const Strigi::RegisteredField* resolutionHeightField;
    const Strigi::RegisteredField* resolutionWidthField;
    const Strigi::RegisteredField* frameRateField;
    const Strigi::RegisteredField* videoCodecField;
    const Strigi::RegisteredField* audioCodecField;

    const char* name() const {
        return "AviThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new AviThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
