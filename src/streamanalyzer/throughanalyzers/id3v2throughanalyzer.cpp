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
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include <iostream>
#include <cstring>
using namespace Strigi;
using namespace std;

#define NMM_PROPOSAL "http://www.semanticdesktop.org/ontologies/nmm#"

const string ID3V2ThroughAnalyzerFactory::titleFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
const string ID3V2ThroughAnalyzerFactory::artistFieldName("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
const string ID3V2ThroughAnalyzerFactory::albumFieldName(NMM_PROPOSAL "musicAlbum");
const string ID3V2ThroughAnalyzerFactory::composerFieldName(NMM_PROPOSAL "composer");
const string ID3V2ThroughAnalyzerFactory::genreFieldName(NMM_PROPOSAL "genre");
const string ID3V2ThroughAnalyzerFactory::trackNumberFieldName(NMM_PROPOSAL "trackNumber");
const string ID3V2ThroughAnalyzerFactory::albumTrackCountFieldName("http://freedesktop.org/standards/xesam/1.0/core#albumTrackCount");
const string ID3V2ThroughAnalyzerFactory::discNumberFieldName(NMM_PROPOSAL "setNumber");

const string
    typePropertyName(
	"http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
    fullnamePropertyName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname"),

    musicClassName(
	NMM_PROPOSAL "MusicPiece"),
    albumClassName(
	NMM_PROPOSAL "MusicAlbum"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");
#undef NMM_PROPOSAL

void
ID3V2ThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField = r.registerField(titleFieldName);
    artistField = r.registerField(artistFieldName);
    albumField = r.registerField(albumFieldName);
    genreField = r.registerField(genreFieldName);
    composerField = r.registerField(composerFieldName);
    trackNumberField = r.registerField(trackNumberFieldName);  //FIXME:id3 track numbers can look like this: 1/10
    albumTrackCountField = r.registerField(albumTrackCountFieldName);
    discNumberField = r.registerField(discNumberFieldName);  //FIXME:id3 disc numbers can looklike this: 1/2

    typeField = r.typeField;
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
    return (((int32_t)b[0])<<24) + (((int32_t)b[1])<<16)
            + (((int32_t)b[2])<<8) + ((int32_t)b[3]);
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
    if (size < 0 || size > 100000) return in;
    size += 10; // add the size of the header

    // read the entire tag
    nread = in->read(buf, size, size);
    in->reset(0);
    if (nread != size || !indexable) {
        return in;
    }
    indexable->addValue(factory->typeField, musicClassName);
    
    string album, disc;
    
    const char* p = buf + 10;
    buf += size;
    while (p < buf && *p) {
        size = readSize((unsigned char*)p+4, async);
        if (size <= 0 || size > (buf-p)-11) {
            // cerr << "size < 0: " << size << endl;
            return in;
        }
        if (p[10] == 0 || p[10] == 3) { // text is ISO-8859-1 or utf8
            if (strncmp("TIT2", p, 4) == 0) {
                indexable->addValue(factory->titleField,
                    string(p+11, size-1));
            } else if (strncmp("TPE1", p, 4) == 0) {
		string artistUri = indexable->newAnonymousUri();
		
                indexable->addValue(factory->artistField, artistUri);
		indexable->addTriplet(artistUri, typePropertyName, contactClassName);
		indexable->addTriplet(artistUri, fullnamePropertyName, string(p+11, size-1));
            } else if (strncmp("TALB", p, 4) == 0) {
                album = string(p+11, size-1);
            } else if (strncmp("TCON", p, 4) == 0) {
                indexable->addValue(factory->genreField,
                    string(p+11, size-1));
            } else if (strncmp("TCOM", p, 4) == 0) {
		string composerUri = indexable->newAnonymousUri();
		
                indexable->addValue(factory->composerField, composerUri);
		indexable->addTriplet(composerUri, typePropertyName, contactClassName);
		indexable->addTriplet(composerUri, fullnamePropertyName, string(p+11, size-1));
            } else if (strncmp("TRCK", p, 4) == 0) {
                indexable->addValue(factory->trackNumberField, string(p+11, size-1));
            } else if (strncmp("TPOS", p, 4) == 0) {
                disc = string(p+11, size-1);
            }
        }
        p += size + 10;
    }
    
    if(album.size()>0) {
	string albumUri = indexable->newAnonymousUri();
	
	indexable->addValue(factory->albumField, albumUri);
	indexable->addTriplet(albumUri, typePropertyName, albumClassName);
	indexable->addTriplet(albumUri, factory->titleFieldName, album);
	if(disc.size()>0)
	  indexable->addTriplet(albumUri, factory->discNumberFieldName, disc);
    }
    return in;
}
bool
ID3V2ThroughAnalyzer::isReadyWithStream() {
    return true;
}
