/* This file is part of Strigi Desktop Search
 *
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

#define STRIGI_IMPORT_API
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <strigi/rdfnamespaces.h>
#include <xine.h>
#include <cstring>
#include <iostream>
using namespace Strigi;
using namespace std;

class XineEndAnalyzerFactory;

class STRIGI_PLUGIN_API XineEndAnalyzer : public StreamEndAnalyzer {
private:
    AnalysisResult* result;
    const XineEndAnalyzerFactory* factory;
    xine_t *engine;
    xine_audio_port_t *audiodrv;
    xine_video_port_t *videodrv;

public:
    XineEndAnalyzer(const XineEndAnalyzerFactory* f) :factory(f) {
      if ((engine = xine_new())) {
        xine_init(engine);

        audiodrv = xine_open_audio_driver(engine, NULL, NULL);
        videodrv = xine_open_video_driver(engine, NULL, XINE_VISUAL_TYPE_NONE, NULL);

        if (!audiodrv || !videodrv)
          xine_exit (engine);
      }
    }
    
    ~XineEndAnalyzer() {
      if (engine) {
        if (audiodrv)
          xine_close_audio_driver(engine, audiodrv);
        if (videodrv)
          xine_close_video_driver(engine, videodrv);
        xine_exit(engine);
      }
    }
    
    const char* name() const {
        return "XineEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(AnalysisResult& idx, ::InputStream* in);
};

class STRIGI_PLUGIN_API XineEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class XineEndAnalyzer;
private:
    StreamEndAnalyzer* newInstance() const {
        return new XineEndAnalyzer(this);
    }
    const char* name() const {
        return "XineEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    const RegisteredField* durationProperty;
    const RegisteredField* widthProperty;
    const RegisteredField* heightProperty;
    const RegisteredField* frameRateProperty;
    const RegisteredField* codecProperty;
    const RegisteredField* bitrateProperty;
    const RegisteredField* channelsProperty;
    const RegisteredField* samplerateProperty;
    const RegisteredField* titleProperty;
    const RegisteredField* commentProperty;
    const RegisteredField* typeProperty;
};

const string
  videoClassName = 
    NFO "Video",
  audioClassName = 
    NFO "Audio",
  musicPieceClassName = 
    NMM_DRAFT "MusicPiece",

  titlePropertyName = 
    NIE "title",
  commentPropertyName = 
    NIE "comment",
    
  sampleratePropertyName = 
    NFO "sampleRate",
  codecPropertyName = 
    NFO "codec",
  channelsPropertyName = 
    NFO "channels",
  bitratePropertyName = 
    NFO "averageBitrate",
  durationPropertyName = 
    NFO "duration",
  widthPropertyName = 
    NFO "width",
  heightPropertyName = 
    NFO "height",
  frameRatePropertyName = 
    NFO "frameRate";

void
XineEndAnalyzerFactory::registerFields(FieldRegister& r) {
  durationProperty = r.registerField(durationPropertyName);
  widthProperty = r.registerField(widthPropertyName);
  heightProperty = r.registerField(heightPropertyName);
  frameRateProperty = r.registerField(frameRatePropertyName);
  codecProperty = r.registerField(codecPropertyName);
  bitrateProperty = r.registerField(bitratePropertyName);
  typeProperty = r.typeField;
  channelsProperty = r.registerField(channelsPropertyName);
  samplerateProperty = r.registerField(sampleratePropertyName);
  titleProperty = r.registerField(titlePropertyName);
  commentProperty = r.registerField(commentPropertyName);
}

//Have to detect here all supported formats because there's no way to pass this to xine
bool
XineEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return headersize>=12 && (
           !strncmp(header, "FLV", 3) // FLV
        || readLittleEndianUInt32(header) == 0x75b22630 // ASF/WMA/WMV
        || (!strncmp(header, "RIFF", 4) && !strncmp(header+8, "AVI ", 4)) // AVI
        || readLittleEndianUInt32(header) == 0xa3df451a // Matroska/MKV/MKA
        || !strncmp(header+4, "ftyp3gp", 7) // 3GPP
        || !strncmp(header+4, "ftypisom", 8) || !strncmp(header+4, "ftypmp42", 8) // MOV
        || !strncmp(header+4, "ftypMSNV", 8) || !strncmp(header+4, "ftypM4", 6) // MOV
        || (!strncmp(header, "OggS", 4) && strncmp(header+29, "vorbis", 6)) // Any ogg apart from Vorbis, which is handled by an internal analyzer
        || readLittleEndianUInt32(header) == 0x10ff3f47 // MPG
        || readLittleEndianUInt32(header) == 0xb3010000 // MPG
        || readLittleEndianUInt32(header) == 0xba010000 // MPG
        );
}

/*
Left unused:
  XINE_STREAM_INFO_SEEKABLE
  XINE_STREAM_INFO_VIDEO_RATIO
  XINE_STREAM_INFO_VIDEO_CHANNELS
  XINE_STREAM_INFO_VIDEO_STREAMS
  XINE_STREAM_INFO_VIDEO_FOURCC
  XINE_STREAM_INFO_VIDEO_HANDLED
  XINE_STREAM_INFO_AUDIO_BITS
  XINE_STREAM_INFO_AUDIO_FOURCC
  XINE_STREAM_INFO_AUDIO_HANDLED
  XINE_STREAM_INFO_HAS_CHAPTERS
  XINE_STREAM_INFO_IGNORE_VIDEO
  XINE_STREAM_INFO_IGNORE_AUDIO
  XINE_STREAM_INFO_IGNORE_SPU
  XINE_STREAM_INFO_VIDEO_HAS_STILL
  XINE_STREAM_INFO_MAX_AUDIO_CHANNEL
  XINE_STREAM_INFO_MAX_SPU_CHANNEL
  XINE_STREAM_INFO_AUDIO_MODE
  XINE_STREAM_INFO_SKIPPED_FRAMES
  XINE_STREAM_INFO_DISCARDED_FRAMES
  XINE_STREAM_INFO_VIDEO_AFD
  XINE_STREAM_INFO_DVD_TITLE_NUMBER
  XINE_STREAM_INFO_DVD_TITLE_COUNT
  XINE_STREAM_INFO_DVD_CHAPTER_NUMBER
  XINE_STREAM_INFO_DVD_CHAPTER_COUNT
  XINE_STREAM_INFO_DVD_ANGLE_NUMBER
  XINE_STREAM_INFO_DVD_ANGLE_COUNT

  XINE_META_INFO_ARTIST
  XINE_META_INFO_GENRE
  XINE_META_INFO_ALBUM
  XINE_META_INFO_YEAR
  XINE_META_INFO_SYSTEMLAYER
  XINE_META_INFO_INPUT_PLUGIN
  XINE_META_INFO_CDINDEX_DISCID
  XINE_META_INFO_TRACK_NUMBER
*/

signed char
XineEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream* in) {

  xine_stream_t *stream;

  int posstream, postime, lengthtime;
  
  string filename;
  
  if ((ar.depth()==0) && (ar.path().substr(0,7) == "file://")) 
    filename = ar.path().substr(7);
  else
    filename = ar.path();
  
  if (!(stream = xine_stream_new(engine, audiodrv, videodrv)))
    return -1;

  if (!xine_open(stream, filename.c_str())) {
    xine_dispose(stream);
    return 0;
  }

  bool audio = xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_AUDIO);

  if (xine_get_pos_length(stream, &posstream, &postime, &lengthtime)) {
    if (lengthtime > 0)
      ar.addValue(factory->durationProperty, lengthtime / 1000); //duration in seconds
  }

  int bitrate = xine_get_stream_info (stream, XINE_STREAM_INFO_BITRATE);
  if (bitrate > 0)
    ar.addValue(factory->bitrateProperty, bitrate);

// video properties

  if ( xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_VIDEO) ) {
    ar.addValue(factory->typeProperty, videoClassName);
    
    int width = xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_WIDTH);
    int height  = xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
    if (width > 0 && height > 0) {
      ar.addValue(factory->widthProperty, width);
      ar.addValue(factory->heightProperty, height);
    }
    
    int duration = xine_get_stream_info(stream, XINE_STREAM_INFO_FRAME_DURATION);
    if ( duration > 0)
      ar.addValue(factory->frameRateProperty, 90000 / duration);
    
    const char *videocodec = xine_get_meta_info(stream, XINE_META_INFO_VIDEOCODEC);
    if (videocodec)
      ar.addValue(factory->codecProperty, videocodec, strlen(videocodec));

    // Somehow bitrate always ends up being 0 :(
    int bitrate = xine_get_stream_info (stream, XINE_STREAM_INFO_VIDEO_BITRATE);
    if (bitrate > 0)
      ar.addValue(factory->bitrateProperty, bitrate);
  } else if (audio) {
    ar.addValue(factory->typeProperty, musicPieceClassName);
    ar.addValue(factory->typeProperty, audioClassName);
  }

// audio properties

  if (audio) {
    // Somehow bitrate always ends up being 0 :(
    int bitrate = xine_get_stream_info (stream, XINE_STREAM_INFO_AUDIO_BITRATE);
    if (bitrate > 0)
      ar.addValue(factory->bitrateProperty, bitrate);

    const char *audiocodec = xine_get_meta_info(stream, XINE_META_INFO_AUDIOCODEC);
    if (audiocodec)
      ar.addValue(factory->codecProperty, audiocodec, strlen(audiocodec));
    
    int channels = xine_get_stream_info(stream, XINE_STREAM_INFO_AUDIO_CHANNELS);
    if (channels>0)
      ar.addValue(factory->channelsProperty, channels);

    int samplerate = xine_get_stream_info(stream, XINE_STREAM_INFO_AUDIO_SAMPLERATE);
    if (samplerate>0)
      ar.addValue(factory->samplerateProperty, samplerate);
  }

// tags

  const char *title = xine_get_meta_info(stream, XINE_META_INFO_TITLE);
  if (title)
    ar.addValue(factory->titleProperty, title);

  const char *comment = xine_get_meta_info(stream, XINE_META_INFO_COMMENT);
  if (comment)
    ar.addValue(factory->commentProperty, comment);
  
  xine_dispose(stream);
  return 0;
}

/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        list<StreamEndAnalyzerFactory*> af;
        af.push_back(new XineEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
