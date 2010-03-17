/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias Pfeiffer <tgpfeiffer@web.de>
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

#include "oggthroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include "../rdfnamespaces.h"
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

    musicClassName(
	NMM_DRAFT "MusicPiece"),
    albumClassName(
	NMM_DRAFT "MusicAlbum"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");

void
OggThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    fields["title"] = r.registerField(titlePropertyName);
    albumField = r.registerField(NMM_DRAFT "musicAlbum");
    artistField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
    fields["genre"] = r.registerField(NMM_DRAFT "genre");
    fields["codec"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
    composerField = r.registerField(NMM_DRAFT "composer");
    performerField = r.registerField(NMM_DRAFT "performer");
    fields["date"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
    fields["description"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#description");
    fields["tracknumber"] = r.registerField(NMM_DRAFT "trackNumber");


    fields["version"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#version");
    fields["isrc"] = r.registerField(NMM_DRAFT "internationalStandardRecordingCode");
    fields["copyright"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright");
    fields["license"] = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license");

// ogg spec fields left unimplemented: ORGANIZATION, LOCATION, CONTACT

    fields["type"] = r.typeField;
}

void
OggThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}
InputStream*
OggThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in) {
        return in;
    }

    const char* buf;
    // read 1024 initially
    int32_t nreq = 1024;
    int32_t nread = in->read(buf, nreq, nreq);
    in->reset(0);
    // check the header for signatures
    // the first ogg page starts at position 0, the second at position 58
    if (nread < nreq || strcmp("OggS", buf) || strcmp("vorbis", buf+29)
            || strcmp("OggS", buf+58)) {
        return in;
    }
    // read the number of page segments at 58 + 26
    unsigned char segments = (unsigned char)buf[84];
    if (85 + segments >= nread) {
        // this cannot be a good vorbis file: the initial page is too large
        return in;
    }

    // read the sum of page segment sizes 
    int psize = 0;
    for (int i=0; i<segments; ++i) {
        psize += (unsigned char)buf[85+i];
    }
    // read the entire first two pages
    nreq = 85 + segments + psize;
    nread = in->read(buf, nreq, nreq);
    in->reset(0);
    if (nread < nreq) {
        return in;
    }
    // we have now read the second Ogg Vorbis header containing the comments
    // get a pointer to the first page segment in the second page
    const char* p2 = buf + 85 + segments;
    // check the header of the 'vorbis' page
    if (psize < 15 || strncmp(p2 + 1, "vorbis", 6)) {
        return in;
    }
    // get a pointer to the end of the second page
    const char* end = p2 + psize;
    uint32_t size = readLittleEndianUInt32(p2+7);
    // advance to the position of the number of fields and read it
    p2 += size + 11;
    if (p2 + 4 > end) {
        return in;
    }
    uint32_t nfields = readLittleEndianUInt32(p2);
    // read all the comments
    p2 += 4;
    for (uint32_t i = 0; p2 < end && i < nfields; ++i) {
        // read the comment length
        size = readLittleEndianUInt32(p2);
        p2 += 4;
        if (size <= (uint32_t)(end - p2)) {
            uint32_t eq = 1;
            while (eq < size && p2[eq] != '=') eq++;
            if (size > eq) {
                string name(p2, eq);
                // convert field name to lower case
                const string::size_type length = name.length();
                for(string::size_type k=0; k!=length; ++k) {
                    name[k] = (char)std::tolower(name[k]);
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
            return in;
        }
        p2 += size;
    }
    // set the "codec" value
    indexable->addValue(factory->fields.find("codec")->second, "Ogg/Vorbis");
    indexable->addValue(factory->fields.find("type")->second, musicClassName);
    return in;
}
bool
OggThroughAnalyzer::isReadyWithStream() {
    return true;
}
