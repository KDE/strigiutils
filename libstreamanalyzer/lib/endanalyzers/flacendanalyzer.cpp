/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias Pfeiffer <tgpfeiffer@web.de>
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

#include "flacendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include "../rdfnamespaces.h"
#include <strigi/stringstream.h>
#include <iostream>
#include <cctype>
#include <cstring>
using namespace Strigi;
using namespace std;

const string
    typePropertyName(
	"http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
    fullnamePropertyName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname"),
    titlePropertyName(
	"http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title"),
    albumTrackCountPropertyName(
	NMM_DRAFT "albumTrackCount"),
    albumGainPropertyName(
	NMM_DRAFT "albumGain"),
    albumPeakGainPropertyName(
	NMM_DRAFT "albumPeakGain"),
    albumArtistPropertyName(
	NMM_DRAFT "albumArtist"),
    musicBrainzAlbumIDPropertyName(
	NMM_DRAFT "musicBrainzAlbumID"),
    discNumberPropertyName(
	NMM_DRAFT "setNumber"),

    musicClassName(
	NMM_DRAFT "MusicPiece"),
    albumClassName(
	NMM_DRAFT "MusicAlbum"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");
/*
TODO:
CUESHEET
FLAC album art: put it where it belongs. Needs a NIE/NMM decision
OGG Vorbis comment album art
More tags: http://lists.xiph.org/pipermail/vorbis-dev/attachments/20090716/e2db9203/attachment-0001.xls

CodecVersion
MBArtistID, MBAlbumArtistID need a nmm:Artist class. needs to be resolved at nepomuk level.
*/

// OGG Vorbis spec fields left unimplemented because they are too ambiguous: Organization, Location, Contact

void
FlacEndAnalyzerFactory::registerFields(FieldRegister& r) {
    sampleRateField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate");
    channelsField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels");
    durationField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration");
    bitsPerSampleField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample");
    sampleCountField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount");
    bitRateField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate");

    fields["title"] = r.registerField(titlePropertyName);
    albumField = r.registerField(NMM_DRAFT "musicAlbum");
    artistField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
    fields["genre"] = r.registerField(NMM_DRAFT "genre");
    codecField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
    composerField = r.registerField(NMM_DRAFT "composer");
    performerField = r.registerField(NMM_DRAFT "performer");
    publisherField = r.registerField(NCO "publisher");
    fields["date"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
    descriptionField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#description");
    fields["description"] = descriptionField;
    fields["comment"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");    
    fields["tracknumber"] = r.registerField(NMM_DRAFT "trackNumber");
    fields["mbtrackid"] = r.registerField(NMM_DRAFT "musicBrainzTrackID");

    fields["replaygain_track_gain"] = r.registerField(NMM_DRAFT "trackGain");
    fields["replaygain_track_peak"] = r.registerField(NMM_DRAFT "trackPeakGain");

    fields["version"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#version");
    fields["isrc"] = r.registerField(NMM_DRAFT "internationalStandardRecordingCode");
    fields["copyright"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright");
    fields["license"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license");

    typeField = r.typeField;
}

bool
FlacEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
  return (headersize>=8) && (readLittleEndianUInt32(header) == 0x43614c66)	// Check for flac stream marker
      && ((readLittleEndianUInt32(header+4) & 0xFFFFFF7F) == 0x22000000); // check for mandatory StreamInfo block header 
}

inline
void
addStatement(AnalysisResult& indexable, string& subject, const string& predicate, const string& object) {
  if (subject.empty())
    subject = indexable.newAnonymousUri();
  indexable.addTriplet(subject, predicate, object);
}

signed char
FlacEndAnalyzer::analyze(Strigi::AnalysisResult& indexable, Strigi::InputStream* in) {
    if(!in)
      return -1;
    
    const char* buf;
    char    blocktype;
    int32_t nreq = 8;
    int32_t nread = in->read(buf, nreq, nreq);
    uint32_t blocksize = 4;
    uint32_t duration = 0;
    char albumArtNum = '\0';
    
    do {
      blocktype = buf[blocksize];
      blocksize = readBigEndianUInt32(buf+blocksize)& 0xFFFFFF;
      
      //if this isn't the last block, read the header of the next block as well
      nreq = (blocktype & 0x80 ? blocksize : blocksize+4);
	
      nread = in->read(buf, nreq, nreq);
      if (nread!=nreq)
        return -1;
      
      if((blocktype&0x7F)==0) { // StreamInfo block
	if (blocksize<18) //StreamInfo block must contain at least 18 bytes
	  return -1;
	
	uint32_t sampleRate = readBigEndianUInt32(buf+2+2+3+3)>>12;
	indexable.addValue(factory->sampleRateField, sampleRate);
	indexable.addValue(factory->channelsField, 1 + ((buf[2+2+3+3+2]>>1) & 7) );
	indexable.addValue(factory->bitsPerSampleField, 1 + ((readBigEndianUInt16(buf+2+2+3+3+2)>>4) & 31) );
	
	uint64_t sampleCount = (readBigEndianUInt64(buf+2+2+3+3+3)>>24) & 0xFFFFFFFFFULL;
	if (sampleCount) {
	  indexable.addValue(factory->sampleCountField, (double) sampleCount ); //FIXME: no int64 support in addValue :(
	  duration = (uint32_t)(sampleCount/sampleRate);
	  indexable.addValue(factory->durationField, duration);
	}
	
      } else if ((blocktype&0x7F)==6) { // FLAC Picture block	
	if (blocksize<32) // the block with no mime, no description and no picture is 32 bytes long
	  return -1;
	  
	uint32_t mimelen = readBigEndianUInt32(buf+4);
	if (12+mimelen>blocksize) // can the block hold mime string and description length?
	   return -1;
	
	const char *p = buf+8+mimelen;
	uint32_t desclen = readBigEndianUInt32(p);
	if (12+mimelen+desclen+20>blocksize) // can the block hold picture and description?
	   return -1;
	string description(p+4, desclen);
	
	p += 4+desclen+16;
	uint32_t piclen = readBigEndianUInt32(p);
	
	if( p+4+piclen>blocksize+buf) // can the block contain the whole picture?
	  return -1;
	
	StringInputStream picstream(p+4, piclen, false);
	string picname;
	picname = (char)('0'+albumArtNum++);
	indexable.indexChild(picname,indexable.mTime(), &picstream);
        if (desclen && indexable.child()) {
          indexable.child()->addValue(factory->descriptionField, description);
        }
	
      } else if ((blocktype&0x7F)==4) { // Vorbis Comment block
	const char *p2 = buf + 4 + readLittleEndianUInt32(buf); //skip vendor string. maybe put it into metadata as soon as there's some place for it
	const char *end = buf + blocksize;
	uint32_t nfields = readLittleEndianUInt32(p2);
	string albumUri;
	
	// read all the comments
	p2 += 4;
	for (uint32_t i = 0; p2 < end && i < nfields; ++i) {
	    // read the comment length
	    uint32_t size = readLittleEndianUInt32(p2);
	    p2 += 4;
	    if (size <= (uint32_t)(end - p2)) {
		uint32_t eq = 1;
		while (eq < size && p2[eq] != '=') eq++;
		if (size > eq) {
		  
		    string name(p2, eq);
		    // convert field name to lower case
                    const size_t length = name.length();
                    for(size_t k=0; k!=length; ++k) {
                        name[k] = (char)std::tolower(name[k]);
		    }
		    
		    // check if we can handle this field and if so handle it
		    map<string, const RegisteredField*>::const_iterator iter
			= factory->fields.find(name);
		    const string value(p2+eq+1, size-eq-1);
		    
		    if (iter != factory->fields.end()) {
			indexable.addValue(iter->second, value);
		    } else if(name=="artist") {
			const string artistUri( indexable.newAnonymousUri() );
		    
			indexable.addValue(factory->artistField, artistUri);
			indexable.addTriplet(artistUri, typePropertyName, contactClassName);
			indexable.addTriplet(artistUri, fullnamePropertyName, value);
		    } else if(name=="lyrics") {
                        indexable.addText(value.c_str(),
                                          (int32_t)value.length());
		    } else if(name=="albumartist") {
			const string albumArtistUri( indexable.newAnonymousUri() );
			addStatement(indexable, albumUri, albumArtistPropertyName, albumArtistUri);
			indexable.addTriplet(albumArtistUri, typePropertyName, contactClassName);
			indexable.addTriplet(albumArtistUri, fullnamePropertyName, value);			
		    } else if(name=="album") {
			addStatement(indexable, albumUri, titlePropertyName, value);
		    } else if(name=="mbalbumid") {
			addStatement(indexable, albumUri, musicBrainzAlbumIDPropertyName, value);
		    } else if(name=="trackcount") {
			addStatement(indexable, albumUri, albumTrackCountPropertyName, value);
		    } else if(name=="replaygain_album_gain") {
			addStatement(indexable, albumUri, albumGainPropertyName, value);
		    } else if(name=="replaygain_album_peak") {
			addStatement(indexable, albumUri, albumPeakGainPropertyName, value);
		    } else if(name=="discno") {
			addStatement(indexable, albumUri, discNumberPropertyName, value);
		    } else if(name=="composer") {
			const string composerUri( indexable.newAnonymousUri() );

			indexable.addValue(factory->composerField, composerUri);
			indexable.addTriplet(composerUri, typePropertyName, contactClassName);
			indexable.addTriplet(composerUri, fullnamePropertyName, value);
		    } else if(name=="publisher") {
			const string publisherUri( indexable.newAnonymousUri() );

			indexable.addValue(factory->publisherField, publisherUri);
			indexable.addTriplet(publisherUri, typePropertyName, contactClassName);
			indexable.addTriplet(publisherUri, fullnamePropertyName, value);
		    } else if(name=="performer") {
			const string performerUri( indexable.newAnonymousUri() );

			indexable.addValue(factory->performerField, performerUri);
			indexable.addTriplet(performerUri, typePropertyName, contactClassName);
			indexable.addTriplet(performerUri, fullnamePropertyName, value);
		    }
		}
	    } else {
		m_error = "problem with tag size";
		return -1;
	    }
	    p2 += size;
	}
	
	if(!albumUri.empty()) {
	  indexable.addValue(factory->albumField, albumUri);
	  indexable.addTriplet(albumUri, typePropertyName, albumClassName);
	}

	// set the "codec" value
	indexable.addValue(factory->codecField, "FLAC");
	indexable.addValue(factory->typeField, musicClassName);
      }
    } while( !(blocktype & 0x80) );

    int64_t in_size = in->size();
    if(in_size>=0 && duration>0)
      indexable.addValue(factory->bitRateField,(uint32_t)(8*((in_size-in->position())/duration)));

    return 0;
}
