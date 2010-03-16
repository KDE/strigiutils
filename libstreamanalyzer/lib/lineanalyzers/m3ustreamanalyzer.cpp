/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 * Copyright (C) 2007 Tim Beaulen <tbscope@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *  $Id$
 */

#include "m3ustreamanalyzer.h"
#include <strigi/fieldtypes.h>
#include <strigi/analysisresult.h>
#include <strigi/streamlineanalyzer.h>
#include <string>
#include <cstring>

// AnalyzerFactory
void M3uLineAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) 
{
// track list length is easily obtained via API
//    tracksField = reg.registerField();
    trackPathField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#hasLogicalPart");
    m3uTypeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#formatSubtype");
    typeField = reg.typeField;

    addField(trackPathField);
    addField(m3uTypeField);
    addField(typeField);
}

// Analyzer
void M3uLineAnalyzer::startAnalysis(Strigi::AnalysisResult* i) 
{
    extensionOk = i->extension() == "m3u" || i->extension() == "M3U";

    analysisResult = i;
    line = 0;
    count = 0;
}

void M3uLineAnalyzer::handleLine(const char* data, uint32_t length) 
{
    if (!extensionOk) 
        return;
    
    ++line;

    if (length == 0)
        return;

    if (*data != '#') {

	//FIXME: either get rid of this or replace with NIE equivalent
        //if (line == 1)
        //    analysisResult->addValue(factory->m3uTypeField, "simple");

        // TODO: Check for a valid url with QUrl
        analysisResult->addValue(factory->trackPathField, std::string(data, length));

        ++count;
    } else if (line == 1 && strncmp(data, "#EXTM3U", 7) == 0) {      
	//FIXME: either get rid of this or replace with NIE equivalent
        //analysisResult->addValue(factory->m3uTypeField, "extended");
    } 
}

bool M3uLineAnalyzer::isReadyWithStream() 
{
    // we can analyze each line and are only done if the extension is not ok
    return !extensionOk;
}

void M3uLineAnalyzer::endAnalysis(bool complete)
{
    // tracksField has not been initialized, so don't use it
    //if (complete && extensionOk)
        //analysisResult->addValue(factory->tracksField, count);
    if (complete && extensionOk)
        analysisResult->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MediaList");

}

