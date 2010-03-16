/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Egon Willighagen <egonw@users.sf.net>
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

#include "namespaceharvestersaxanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <set>

using namespace std;
using namespace Strigi;

void
NamespaceHarvesterSaxAnalyzerFactory::registerFields(FieldRegister &reg) {
    usesNamespaceField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#usesNamespace");
    addField(usesNamespaceField);
}

void
NamespaceHarvesterSaxAnalyzer::startAnalysis(AnalysisResult *i) {
    assert(i != 0);
    analysisResult = i;
    ready = false;

    usedNamespaces.clear();
}

void
NamespaceHarvesterSaxAnalyzer::endAnalysis(bool /*complete*/) {
    set<string>::iterator i; // Iterator for looping over list elements
    for (i = usedNamespaces.begin(); i != usedNamespaces.end(); ++i) {
	//FIXME: either get rid of this or replace with NIE equivalent
        //analysisResult->addValue(factory->usesNamespaceField, *i);
    }

    analysisResult = 0;
    usedNamespaces.clear();
    ready = true;
}

void
NamespaceHarvesterSaxAnalyzer::startElement(const char *localname,
        const char *prefix, const char *uri, int nb_namespaces,
        const char **namespaces, int nb_attributes, int nb_defaulted,
        const char **attributes) {
    assert(analysisResult != 0);
    if (uri) {
        usedNamespaces.insert(uri);
    }
}

void
NamespaceHarvesterSaxAnalyzer::endElement(const char *localname,
        const char *prefix, const char *uri) {
}

void
NamespaceHarvesterSaxAnalyzer::characters(const char *data, uint32_t length) {
    assert(analysisResult != 0);
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamSaxAnalyzerFactory*>
    streamSaxAnalyzerFactories() const {
        list<StreamSaxAnalyzerFactory*> af;
        af.push_back(new NamespaceHarvesterSaxAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
