/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Christopher Blauvelt <cblauvelt@gmail.com>
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
#ifndef MPEGENDANALYZER_H
#define MPEGENDANALYZER_H

#include "streamendanalyzer.h"
#include "fieldtypes.h"
#include <map>

class MpegEndAnalyzerFactory;

class MpegEndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    Strigi::AnalysisResult* indexable;
    const MpegEndAnalyzerFactory* factory;
    
    // MPEG information
    int horizontal_size;
    int vertical_size;
    int aspect_ratio;
    int bitrate;
    float frame_rate;

    int mpeg_version;
    int audio_type;

    long start_time;
    long end_time;

    
    // Frame-rate table from libmpeg3
    float frame_rate_table[16];
    
    static const uint16_t sequence_start = 0x01b3;
    static const uint16_t ext_sequence_start = 0x01b5;
    static const uint16_t gop_start = 0x01b8;
    static const uint16_t audio1_packet = 0x01c0;
    static const uint16_t audio2_packet = 0x01d0;
    static const uint16_t private1_packet = 0x01bd;
    static const uint16_t private2_packet = 0x01bf;


public:
    MpegEndAnalyzer(const MpegEndAnalyzerFactory* f) :factory(f)
    {
        this->frame_rate_table[0] = 0.0;   /* Pad */
        //Official frame rates
        this->frame_rate_table[1] = (float)24000.0/1001.0;
        this->frame_rate_table[2] = (float)24.0;
        this->frame_rate_table[3] = (float)25.0;
        this->frame_rate_table[4] = (float)30000.0/1001.0;
        this->frame_rate_table[5] = (float)30.0;
        this->frame_rate_table[6] = (float)50.0;
        this->frame_rate_table[7] = (float)60000.0/1001.0;
        this->frame_rate_table[8] = (float)60.0;
        
        // Unofficial economy rates
        this->frame_rate_table[9] = 1;
        this->frame_rate_table[10] = 5;
        this->frame_rate_table[11] = 10;
        this->frame_rate_table[12] = 12;
        this->frame_rate_table[13] = 15;
        this->frame_rate_table[14] = 0;
        this->frame_rate_table[15] = 0;
        
    }
    ~MpegEndAnalyzer() {}
    const char* name() const { return "MpegEndAnalyzer"; }
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(Strigi::AnalysisResult& idx,
                 Strigi::InputStream* in);
    bool readMpeg(Strigi::InputStream* in);
    bool parse_seq(Strigi::InputStream* in);
    bool parse_seq_ext(Strigi::InputStream* in);
    bool parse_private(Strigi::InputStream* in);
    bool parse_audio(Strigi::InputStream* in);
    
};

class MpegEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
friend class MpegEndAnalyzer;
private:
    std::map<std::string, const Strigi::RegisteredField*> fields;
    const char* name() const {
        return "MpegEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new MpegEndAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
