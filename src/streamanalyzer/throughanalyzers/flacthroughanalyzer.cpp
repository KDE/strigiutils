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

#include "flacthroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "textutils.h"
#include <iostream>
#include <cctype>
#include <cstring>
using namespace Strigi;
using namespace std;

#define NMM_PROPOSAL "http://www.semanticdesktop.org/ontologies/nmm#"

const string
    typePropertyName(
	"http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
    fullnamePropertyName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname"),
    titlePropertyName(
	"http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title"),

    musicClassName(
	NMM_PROPOSAL "MusicPiece"),
    albumClassName(
	NMM_PROPOSAL "MusicAlbum"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");

void
FlacThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    fields["title"] = r.registerField(titlePropertyName);
    albumField = r.registerField(NMM_PROPOSAL "musicAlbum");
    artistField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
    fields["genre"] = r.registerField(NMM_PROPOSAL "genre");
    fields["codec"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
    composerField = r.registerField(NMM_PROPOSAL "composer");
    performerField = r.registerField(NMM_PROPOSAL "performer");
    fields["date"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
    fields["description"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#description");
    fields["tracknumber"] = r.registerField(NMM_PROPOSAL "trackNumber");


    fields["version"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#version");
    fields["isrc"] = r.registerField(NMM_PROPOSAL "internationalStandardRecordingCode");
    fields["copyright"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright");
    fields["license"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license");

// ogg spec fields left unimplemented: ORGANIZATION, LOCATION, CONTACT

    fields["type"] = r.typeField;
}

#undef NMM_PROPOSAL

void
FlacThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}
InputStream*
FlacThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in) {
        return in;
    }
    const char* buf;
    char    blocktype;
    int32_t nreq = 8;
    int32_t nread = in->read(buf, nreq, nreq);
    int32_t blocksize = 4;
    // check the header for Flac signature
    // the first ogg page starts at position 0, the second at position 58
    if (nread < nreq || strncmp("fLaC", buf,4)) {
	in->reset(0);
        return in;
    }
    
    //cerr<< "found a flac file!";    
    do {
      blocktype = buf[blocksize];
      blocksize = readBigEndianUInt32(buf+blocksize)& 0xFFFFFF;
      
      //if this isn't the last block, read the header of the next block as well
      nreq = (blocktype & 0x80 ? blocksize : blocksize+4);
	
      nread = in->read(buf, nreq, nreq);
      if (nread!=nreq) {
	in->reset(0);
        return in;
      }
      
      // we are looking for the comments block only
      if ((blocktype&0x7F)==4) {
	const char *p2 = buf + 4 + readLittleEndianUInt32(buf); //skip vendor string. maybe put it into metadata as soon as there's some place for it
	const char *end = buf + blocksize;
	uint32_t nfields = readLittleEndianUInt32(p2);
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
		    const int length = name.length();
		    for(int k=0; k!=length; ++k) {
			name[k] = std::tolower(name[k]);
		    }
		    // check if we can handle this field and if so handle it
		    map<string, const RegisteredField*>::const_iterator iter
			= factory->fields.find(name);
		    string value(p2+eq+1, size-eq-1);
		    if (iter != factory->fields.end()) {
			indexable->addValue(iter->second, value);
		    } else if(name=="artist") {
			string artistUri = indexable->newAnonymousUri();
		    
			indexable->addValue(factory->artistField, artistUri);
			indexable->addTriplet(artistUri, typePropertyName, contactClassName);
			indexable->addTriplet(artistUri, fullnamePropertyName, value);
		    } else if(name=="album") {
			string albumUri = indexable->newAnonymousUri();
			
			indexable->addValue(factory->albumField, albumUri);
			indexable->addTriplet(albumUri, typePropertyName, albumClassName);
			indexable->addTriplet(albumUri, titlePropertyName, value);
		    } else if(name=="composer") {
			string composerUri = indexable->newAnonymousUri();

			indexable->addValue(factory->composerField, composerUri);
			indexable->addTriplet(composerUri, typePropertyName, contactClassName);
			indexable->addTriplet(composerUri, fullnamePropertyName, value);
		    } else if(name=="performer") {
			string performerUri = indexable->newAnonymousUri();

			indexable->addValue(factory->performerField, performerUri);
			indexable->addTriplet(performerUri, typePropertyName, contactClassName);
			indexable->addTriplet(performerUri, fullnamePropertyName, value);
		    }
		}
	    } else {
		cerr << "problem with tag size of " << size << endl;
		in->reset(0);
		return in;
	    }
	    p2 += size;
	}
	// set the "codec" value
	indexable->addValue(factory->fields.find("codec")->second, "FLAC");
	indexable->addValue(factory->fields.find("type")->second, musicClassName);
      }
    } while( !(blocktype & 0x80) );

    in->reset(0);
    return in;
}
bool
FlacThroughAnalyzer::isReadyWithStream() {
    return true;
}
