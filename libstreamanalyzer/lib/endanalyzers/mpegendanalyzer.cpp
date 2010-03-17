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
 
 /* Based on source code from Mpeg KFile plugin by Allan Sandfeld Jensen
  * and libmpeg3.
  */   

#include "mpegendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include "../rdfnamespaces.h"
#include <strigi/textutils.h>
#include <iostream>
using namespace Strigi;
using namespace std;

const string
    videoClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video");

void MpegEndAnalyzerFactory::registerFields(FieldRegister& r) {
    fields["length"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration");
    fields["dimensions.y"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    fields["dimensions.x"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    fields["frame rate"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate");
    fields["video codec"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
    fields["audio codec"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
    fields["aspect ratio"] = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#aspectRatio");
    fields["type"] = r.typeField;

    map<string, const RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        addField(i->second);
    }
}

bool
MpegEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    uint32_t dword = 0;
    
    if(headersize < 9) {
        //cerr << "File was less than nine bytes.  Not long enough" << endl;
        return false;
    }
    dword = readBigEndianUInt32(header);
    
    if(dword == 0x52494646) // == "RIFF"
    {
        dword = readBigEndianUInt32(header+5);
        
        if (dword == 0x43445841) { // == "CDXA"
            //cerr << "CDXA not yet supported" << endl;
            return false;
        }
    }
    else if(dword != 0x000001ba) {
        //cerr << "Not a MPEG-PS file" << endl;
        return false;
    }
    uint16_t packet = readBigEndianUInt16(header);
    if (packet) return false;
    packet = readBigEndianUInt16(header+2);
    return packet == sequence_start || packet == ext_sequence_start
        || packet == private1_packet || packet == private2_packet
        || packet == audio1_packet || packet == audio2_packet
        || packet == 0x01ba;
}

signed char
MpegEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in) {
        return -1;
    }
    
    if(!this->readMpeg(in) ) {
        return -1;
    }
    
    std::map<std::string, const Strigi::RegisteredField*>tempfields
        = factory->fields;
    idx.addValue(tempfields["frame rate"], this->frame_rate);
    idx.addValue(tempfields["dimensions.y"], (int32_t)this->vertical_size);
    idx.addValue(tempfields["dimensions.x"], (int32_t)this->horizontal_size);
    
    if (this->mpeg_version == 1) {
        idx.addValue(tempfields["video codec"], "MPEG-1");
    } else {
        idx.addValue(tempfields["video codec"], "MPEG-2");
    }

    switch (this->audio_type)
    {
        case 1:
            idx.addValue(tempfields["audio codec"], "MP1");
            break;
        case 2:
            idx.addValue(tempfields["audio codec"], "MP2");
            break;
        case 3:
            idx.addValue(tempfields["audio codec"], "MP3");
            break;
        case 5:
            idx.addValue(tempfields["audio codec"], "AC3");
            break;
        case 7:
            idx.addValue(tempfields["audio codec"], "PCM");
            break;
        default:
            idx.addValue(tempfields["audio codec"], "Unknown");
    }
    // MPEG 1 also has an aspect ratio setting, but it works differently,
    // and I am not sure if it is used.
    if (this->mpeg_version == 2) {
        switch (this->aspect_ratio) {
            case 1:
                idx.addValue(tempfields["aspect ratio"], "default");
                break;
            case 2:
                idx.addValue(tempfields["aspect ratio"], "4/3");
                break;
            case 3:
                idx.addValue(tempfields["aspect ratio"], "16/9");
                break;
            case 4:
                idx.addValue(tempfields["aspect ratio"], "2.11/1");
                break;
        }
    }
    idx.addValue(tempfields["type"], videoClassName);
    return 0;
}

bool MpegEndAnalyzer::readMpeg(InputStream* in) {
    if(!in) {
        return false;
    }
    const char *buf;
    this->mpeg_version = 0;
    this->audio_type = 0;
    uint32_t nread = 0;
    
    //Now that we've established that this is an mpeg file (almost),
    //we search for the audio and video elements
    in->reset(0);

    uint16_t packet;
    bool video_found = false, audio_found = false, read_error = false;
    
    // Start searching for MPEG packets
    while((nread = in->read(buf,2,2) ) )
    {
        if(nread != 2)
        {
            read_error = true;
            break;
        }
        
        packet = readBigEndianUInt16(buf);
        if(packet == this->sequence_start)
        {
            if(video_found) break;
            if(this->parse_seq(in) ) video_found = true;
        }
        else if(packet == this->ext_sequence_start)
        {
            this->parse_seq_ext(in);
        }
        else if(packet == this->private1_packet || packet == this->private2_packet)
        {
            this->parse_private(in);
        }
        else if(packet == this->audio1_packet || packet == this->audio2_packet)
        {
            if(audio_found) break;
            if(this->parse_audio(in) ) audio_found = true;
        }
        if (video_found && audio_found) break;
    }
    if(read_error)
    {
        return false;
    }
    else if (this->mpeg_version == 0) {
        //cerr << "No sequence-start found" << endl;
        return false;
    }
    return true;
}

bool MpegEndAnalyzer::parse_seq(InputStream* in)
{
    if(!in) {
        return false;
    }
    const char *buf;
    uint32_t dword = 0,nread = 0;
    
    nread = in->read(buf,4,4);
    if(nread < 4) return false;
    dword = readBigEndianUInt32(buf);
    
    this->horizontal_size = (dword >> 20);
    this->vertical_size = (dword >> 8) & ((1<<12)-1);
    this->aspect_ratio = (dword >> 4) & ((1<<4)-1);
    int framerate_code = dword & ((1<<4)-1);
    this->frame_rate = this->frame_rate_table[framerate_code];
    
    nread = in->read(buf,4,4);
    if(nread < 4) return false;
    dword = readBigEndianUInt32(buf);
    
    this->bitrate = (dword >> 14);
    this->mpeg_version = 1;
    
    return true;
}

bool MpegEndAnalyzer::parse_seq_ext(InputStream* in)
{
    if(!in) {
        return false;
    }
    const char *buf;
    uint32_t dword = 0,nread = 0;
    
    nread = in->read(buf,4,4);
    if(nread < 4) return false;
    dword = readBigEndianUInt32(buf);
    
    uint8_t type = (uint8_t)(dword >> 28);
    if(type == 1 )
    {
        this->mpeg_version = 2;
    }
    return true;
}

bool MpegEndAnalyzer::parse_private(InputStream* in)
{
    if(!in) {
        return false;
    }
    const char *buf;
    uint32_t nread = 0;
    uint8_t subtype = 0;
    
    //skip the first two bytes
    in->skip(2);
    
    nread = in->read(buf,1,1);
    if(nread < 1) return false;
    subtype = (uint8_t)*buf;
    
    subtype = (uint8_t)(subtype >> 4);
    if (subtype == 8)   // AC3
        this->audio_type = 5;
    else
    if (subtype == 10)  // LPCM
        this->audio_type = 7;

    return true;
}

bool MpegEndAnalyzer::parse_audio(InputStream* in)
{
    if(!in) {
        return false;
    }
    const char *buf;
    uint32_t nread = 0;
    uint8_t byte = 0;
    
    //skip the first two bytes (16bits)
    in->skip(2);
    
    bool sync_found = false;
    //anybody know why this is 20?
    for(int i=0; i<20; i++) {
        nread = in->read(buf,1,1);
        if(nread != 1) return false;
        byte = (uint8_t)*buf;
        if (byte == 0xff) {
            nread = in->read(buf,1,1);
            if(nread != 1) return false;
            byte = (uint8_t)*buf;
        
            if ((byte & 0xe0) == 0xe0)
                sync_found = true;
                break;
        }
    }
    if(!sync_found)
    {
        //cerr << "MPEG audio sync not found" << endl;
        return false;
    }
    
    //the sync code was found so parse the audio type
    int layer = ((byte >> 1) & 0x3);
    if (layer == 1)
        this->audio_type = 3;
    else if (layer == 2)
        this->audio_type = 2;
    else if (layer == 3)
        this->audio_type = 1;
    else
    {
        //cerr << "Invalid MPEG audio layer" << endl;
    }
    

    return true;
    
}
