/* This file is part of Strigi Desktop Search, ported from code of:
 * - Rolf Magnus <ramagnus@kde.org> (2003)
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

#include "sidthroughanalyzer.h"
#include <strigi/textutils.h>
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>

#include <iostream>

using namespace std;
using namespace Strigi;

// AnalyzerFactory

void
SidThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    titleField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
    artistField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
    trackNumberField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#albumTrackCount");
    versionField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#version");
    copyrightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright");

    addField(titleField);
    addField(artistField);
    addField(trackNumberField);
    addField(versionField);
    addField(copyrightField);
}

// Analyzer
void
SidThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
SidThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    const char *c;
    int version;
    int num_songs;
    int start_song;
    string title;
    string artist;
    string copyright;

    // read the beginning of the stream and make sure it looks ok
    if (4 != in->read(c, 4, 4)) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint32_t header1 = readLittleEndianUInt32(c);
    
    if (strncmp((char*) &header1, "PSID", 4)) {
        in->reset(0);
        return in;
    }

    //read version
    if (2 != in->read(c, 2, 2)) {
        in->reset(0);
        return in;
    }
    version = readBigEndianUInt16(c);

    //jump to 0xE
    if (8 != in->skip(8)) {
        in->reset(0);
        return in;
    }

    //read number of songs
    if (2 != in->read(c, 2, 2)) {
        in->reset(0);
        return in;
    }
    num_songs = readBigEndianUInt16(c);

    //start song
    if (2 != in->read(c, 2, 2)) {
        in->reset(0);
        return in;
    }
    start_song = readBigEndianUInt16(c);

    //jump to 0x16
    if (4 != in->skip(4)) {
        in->reset(0);
        return in;
    }

    //title
    if (32 != in->read(c, 32, 32)) {
        in->reset(0);
        return in;
    }
    title = c;

    //artist
    if (32 != in->read(c, 32, 32)) {
        in->reset(0);
        return in;
    }
    artist = c;

    //copyright
    if (32 != in->read(c, 32, 32)) {
        in->reset(0);
        return in;
    }
    copyright = c;
    
    // read the data on the 1st icon
    string artistUri = analysisResult->newAnonymousUri();
	
    analysisResult->addValue(factory->artistField, artistUri);
    analysisResult->addTriplet(artistUri,
			       "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
			       "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");
    analysisResult->addTriplet(artistUri,
			       "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname",
			       artist);
    
    analysisResult->addValue( factory->titleField, title );
    analysisResult->addValue( factory->copyrightField, copyright );
    //analysisResult->addValue( factory->trackNumberField, num_songs ); //FIXME: either get rid of this or replace with NIE equivalent
    analysisResult->addValue( factory->versionField, version );
    
    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
SidThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new SidThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
