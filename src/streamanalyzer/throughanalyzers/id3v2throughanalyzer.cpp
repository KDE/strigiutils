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

#include "id3v2throughanalyzer.h"
#include "rdfnamespaces.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "textutils.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>

using namespace Strigi;
using namespace std;

const string
    typePropertyName(
	RDF "type"),
    fullnamePropertyName(
	NCO "fullname"),
    titlePropertyName(
	NIE "title"),
    albumTrackCountPropertyName(
	NMM_DRAFT "albumTrackCount"),
    discNumberPropertyName(
	NMM_DRAFT "setNumber"),
    discCountPropertyName(
	NMM_DRAFT "setCount"),
    
    musicClassName(
	NMM_DRAFT "MusicPiece"),
    albumClassName(
	NMM_DRAFT "MusicAlbum"),
    contactClassName(
	NCO "Contact");

/*
Album Art
UTF16 in Frames
ENCA autodetection of broken encodings
ID3v2.0
play counter:needs nepomuk resolution
refactor as endanalyzer?
actual mp3 support: bitrate and stuff
ID3v1
lyrics
Improve:
  creation date:
  language: support multiple
  Genre
*/

static const string genres[] = {
  "Blues",
  "Classic Rock",
  "Country",
  "Dance",
  "Disco",
  "Funk",
  "Grunge",
  "Hip-Hop",
  "Jazz",
  "Metal",
  "New Age",
  "Oldies",
  "Other",
  "Pop",
  "R&B",
  "Rap",
  "Reggae",
  "Rock",
  "Techno",
  "Industrial",
  "Alternative",
  "Ska",
  "Death Metal",
  "Pranks",
  "Soundtrack",
  "Euro-Techno",
  "Ambient",
  "Trip-Hop",
  "Vocal",
  "Jazz+Funk",
  "Fusion",
  "Trance",
  "Classical",
  "Instrumental",
  "Acid",
  "House",
  "Game",
  "Sound Clip",
  "Gospel",
  "Noise",
  "Alternative Rock",
  "Bass",
  "Soul",
  "Punk",
  "Space",
  "Meditative",
  "Instrumental Pop",
  "Instrumental Rock",
  "Ethnic",
  "Gothic",
  "Darkwave",
  "Techno-Industrial",
  "Electronic",
  "Pop-Folk",
  "Eurodance",
  "Dream",
  "Southern Rock",
  "Comedy",
  "Cult",
  "Gangsta",
  "Top 40",
  "Christian Rap",
  "Pop/Funk",
  "Jungle",
  "Native American",
  "Cabaret",
  "New Wave",
  "Psychedelic",
  "Rave",
  "Showtunes",
  "Trailer",
  "Lo-Fi",
  "Tribal",
  "Acid Punk",
  "Acid Jazz",
  "Polka",
  "Retro",
  "Musical",
  "Rock & Roll",
  "Hard Rock",
  "Folk",
  "Folk/Rock",
  "National Folk",
  "Swing",
  "Fusion",
  "Bebob",
  "Latin",
  "Revival",
  "Celtic",
  "Bluegrass",
  "Avantgarde",
  "Gothic Rock",
  "Progressive Rock",
  "Psychedelic Rock",
  "Symphonic Rock",
  "Slow Rock",
  "Big Band",
  "Chorus",
  "Easy Listening",
  "Acoustic",
  "Humour",
  "Speech",
  "Chanson",
  "Opera",
  "Chamber Music",
  "Sonata",
  "Symphony",
  "Booty Bass",
  "Primus",
  "Porn Groove",
  "Satire",
  "Slow Jam",
  "Club",
  "Tango",
  "Samba",
  "Folklore",
  "Ballad",
  "Power Ballad",
  "Rhythmic Soul",
  "Freestyle",
  "Duet",
  "Punk Rock",
  "Drum Solo",
  "A Cappella",
  "Euro-House",
  "Dance Hall",
  "Goa",
  "Drum & Bass",
  "Club-House",
  "Hardcore",
  "Terror",
  "Indie",
  "BritPop",
  "Negerpunk",
  "Polsk Punk",
  "Beat",
  "Christian Gangsta Rap",
  "Heavy Metal",
  "Black Metal",
  "Crossover",
  "Contemporary Christian",
  "Christian Rock",
  "Merengue",
  "Salsa",
  "Thrash Metal",
  "Anime",
  "Jpop",
  "Synthpop"
};

void
ID3V2ThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    createdField	= r.registerField(NIE "contentCreated");
    subjectField	= r.registerField(NIE "subject");
    titleField		= r.registerField(titlePropertyName);
    descriptionField	= r.registerField(NIE "description");
    artistField		= r.registerField(NCO "creator");
    albumField		= r.registerField(NMM_DRAFT "musicAlbum");
    genreField		= r.registerField(NMM_DRAFT "genre");
    composerField	= r.registerField(NMM_DRAFT "composer");
    performerField	= r.registerField(NMM_DRAFT "performer");
    lyricistField	= r.registerField(NMM_DRAFT "lyricist");
    publisherField	= r.registerField(NCO "publisher");
    languageField	= r.registerField(NIE "language");
    copyrightField	= r.registerField(NIE "copyright");
    trackNumberField	= r.registerField(NMM_DRAFT "trackNumber");  //FIXME:id3 track numbers can look like this: 1/10
    durationField	= r.registerField(NFO "duration");
    typeField		= r.typeField;
}

inline
void
addStatement(AnalysisResult *indexable, string& subject, const string& predicate, const string& object) {
    if (subject.empty())
	subject = indexable->newAnonymousUri();
    indexable->addTriplet(subject, predicate, object);
}

void
ID3V2ThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}

int32_t
readSize(const unsigned char* b, bool async) {
    const signed char* c = (const signed char*)b;
    if (async) {
        if (c[0] < 0 || c[1] < 0 || c[2] < 0 || c[3] < 0) {
            return -1;
        }
        return (((int32_t)b[0])<<21) + (((int32_t)b[1])<<14)
                + (((int32_t)b[2])<<7) + ((int32_t)b[3]);
    }
    return readBigEndianInt32(b);
}
InputStream*
ID3V2ThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in)
        return in;
    // read 10 byte header
    const char* buf;
    int32_t nread = in->read(buf, 10, 10);
    const signed char* sbuf = (const signed char*)buf;
    in->reset(0);
    if (nread != 10 || strncmp("ID3", buf, 3) != 0 // check that it's ID3
            || sbuf[3] < 0 || buf[3] > 4  // only handle version <= 4
            || buf[5] != 0 // we're too dumb too handle flags
            ) {
        return in;
    }
    bool async = buf[3] >= 4;

    // calculate size from 4 syncsafe bytes
    int32_t size = readSize((unsigned char*)buf+6, async);
    if (size < 0 || size > 300000)
	return in;
    size += 10; // add the size of the header

    // read the entire tag
    nread = in->read(buf, size, size);
    in->reset(0);
    if (nread != size || !indexable) {
        return in;
    }
    indexable->addValue(factory->typeField, musicClassName);
    
    string albumUri;

    const char* p = buf + 10;
    buf += size;
    while (p < buf && *p) {
        size = readSize((unsigned char*)p+4, async);
        if (size <= 0 || size > (buf-p)-11) {
            // cerr << "size < 0: " << size << endl;
            return in;
        }

	const string value(p+11, size-1);
        if (p[10] == 0 || p[10] == 3) { // text is ISO-8859-1 or utf8
            if (strncmp("TIT1", p, 4) == 0) {
                indexable->addValue(factory->subjectField, value);
	    } else if (strncmp("TIT2", p, 4) == 0) {
                indexable->addValue(factory->titleField, value);
            } else if (strncmp("TIT3", p, 4) == 0) {
                indexable->addValue(factory->descriptionField, value);
            } else if (strncmp("TLAN", p, 4) == 0) {
                indexable->addValue(factory->languageField, value);
            } else if (strncmp("TCOP", p, 4) == 0) {
                indexable->addValue(factory->copyrightField, value);
            } else if ((strncmp("TDRL", p, 4) == 0) ||
			(strncmp("TDAT", p, 4) == 0) ||
			(strncmp("TYER", p, 4) == 0) ||
			(strncmp("TDRC", p, 4) == 0)) {
                indexable->addValue(factory->createdField, value);
            } else if ((strncmp("TPE1", p, 4) == 0) ||
			(strncmp("TPE2", p, 4) == 0) ||
			(strncmp("TPE3", p, 4) == 0) ||
			(strncmp("TPE4", p, 4) == 0)) {
		string performerUri = indexable->newAnonymousUri();
		
                indexable->addValue(factory->performerField, performerUri);
		indexable->addTriplet(performerUri, typePropertyName, contactClassName);
		indexable->addTriplet(performerUri, fullnamePropertyName, value);
	    } else if ((strncmp("TPUB", p, 4) == 0) ||
			(strncmp("TENC", p, 4) == 0)) {
		string publisherUri = indexable->newAnonymousUri();
		
                indexable->addValue(factory->publisherField, publisherUri);
		indexable->addTriplet(publisherUri, typePropertyName, contactClassName);
		indexable->addTriplet(publisherUri, fullnamePropertyName, value);
            } else if ((strncmp("TALB", p, 4) == 0) ||
			(strncmp("TOAL", p, 4) == 0)) {
		addStatement(indexable, albumUri, titlePropertyName, value);
            } else if (strncmp("TCON", p, 4) == 0) {
                indexable->addValue(factory->genreField, value);
            } else if (strncmp("TLEN", p, 4) == 0) {
                indexable->addValue(factory->durationField, value);
            } else if (strncmp("TEXT", p, 4) == 0) {
		string lyricistUri = indexable->newAnonymousUri();
		
                indexable->addValue(factory->lyricistField, lyricistUri);
		indexable->addTriplet(lyricistUri, typePropertyName, contactClassName);
		indexable->addTriplet(lyricistUri, fullnamePropertyName, value);
            } else if (strncmp("TCOM", p, 4) == 0) {
		string composerUri = indexable->newAnonymousUri();

                indexable->addValue(factory->composerField, composerUri);
		indexable->addTriplet(composerUri, typePropertyName, contactClassName);
		indexable->addTriplet(composerUri, fullnamePropertyName, value);
            } else if (strncmp("TRCK", p, 4) == 0) {
		if (int tnum = atoi(p+11)) { // use p directly instead of value for speed
		    indexable->addValue(factory->trackNumberField, tnum);
		    if (char *pos = (char*)memchr(p+11,'/',size-1)) {
			  if (int tcount = atoi(pos+1)) {
			      ostringstream outs;
			      outs << tcount;
			      addStatement(indexable, albumUri, albumTrackCountPropertyName, outs.str());
			  }
		    }
		}
            } else if (strncmp("TPOS", p, 4) == 0) {
		if (int dnum  = atoi(p+11)) { // use p directly instead of value for speed
		    ostringstream out;
		    out << dnum;
		    addStatement(indexable, albumUri, discNumberPropertyName, out.str());
		    if (char *pos = (char*)memchr(p+11,'/',size-1)) {
			  if (int dcount = atoi(pos+1)) {
			      ostringstream outs;
			      outs << dcount;
			      addStatement(indexable, albumUri, discCountPropertyName, outs.str());
			  }
		    }		    
		}
            }
        }
        p += size + 10;
    }

    if(!albumUri.empty()) {
	indexable->addValue(factory->albumField, albumUri);
	indexable->addTriplet(albumUri, typePropertyName, albumClassName);
    }
    
    return in;
}
bool
ID3V2ThroughAnalyzer::isReadyWithStream() {
    return true;
}
