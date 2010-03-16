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

#include "deblineanalyzer.h"
#include <strigi/analysisresult.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/strigiconfig.h>
#include <strigi/fieldtypes.h>
#include <iostream> 
#include <cstring>

using namespace Strigi;
using namespace std;

void
DebLineAnalyzerFactory::registerFields(FieldRegister& r) {
    nameField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
    versionField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#version");
    summaryField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#description");
    maintainerField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#keyword");
    sectionField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#keyword");
    dependsField = r.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends");
    typeField = r.typeField;

    addField(nameField);
    addField(versionField);
    addField(summaryField);
    addField(maintainerField);
    addField(sectionField);
    addField(dependsField);
    addField(typeField);
}

void 
DebLineAnalyzer::startAnalysis(AnalysisResult* res) {
    // let's assume that it is not .deb file and set isReadyWithStream() to true
    finished=6;
    if (res->fileName()!="control") return;
    res=res->parent();
    if (!res) return;
    if (res->fileName()!="control.tar.gz") return;
    res=res->parent();
    if (!res) return;
    if (strcmp(res->endAnalyzer()->name(),"ArEndAnalyzer")) return ;

    // it is .deb file after all
    result=res;
    finished=0;
    result->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Software");
}
void
DebLineAnalyzer::endAnalysis(bool complete) {
}

void 
DebLineAnalyzer::handleLine(const char* data, uint32_t length)
{
	string line(data,length);
	if (line.find("Package: ",0)==0) { result->addValue(factory->nameField, line.substr(9,line.size())); finished++; }
	if (line.find("Description: ",0)==0) { result->addValue(factory->summaryField, line.substr(13,line.size())); finished++; }
	if (line.find("Version: ")==0) { result->addValue(factory->versionField, line.substr(9,line.size())); finished++; }
	if (line.find("Maintainer: ")==0) { result->addValue(factory->maintainerField, line.substr(12,line.size())); finished++; }
	if (line.find("Section: ")==0) { result->addValue(factory->sectionField, line.substr(9,line.size())); finished++; }
	if (line.find("Depends: ")==0) {
	    size_t start=9;
	    size_t end;
	    do {
		end=line.find(", ",start);
		if (end==string::npos) end=length;
		result->addValue(factory->dependsField, line.substr(start, end-start));
		start=end+2;
	    } while (start<length);
	    finished++;
	}
}

bool
DebLineAnalyzer::isReadyWithStream()
{
    // analysis is finished after all 6 fields were found (name, summary, version, deps, maintainer, section)
    return finished==6; 
}

class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamLineAnalyzerFactory*> 
    streamLineAnalyzerFactories() const {
        list<StreamLineAnalyzerFactory*> af;
        af.push_back(new DebLineAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
