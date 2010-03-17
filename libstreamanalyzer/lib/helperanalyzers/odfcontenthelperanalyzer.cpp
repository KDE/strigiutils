/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Arend van Beelen jr. <arend@auton.nl>
 * 		 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#include "odfcontenthelperanalyzer.h"
#include <strigi/analysisresult.h>
#include <cassert>
#include <cstring>
#include <string>

#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include "../rdfnamespaces.h"

using namespace Strigi;
using namespace std;

void OdfContentHelperAnalyzer::startAnalysis(AnalysisResult &r) {
    result = &r;
}

void OdfContentHelperAnalyzer::endAnalysis(bool /*complete*/) {
    result = NULL;
}

void OdfContentHelperAnalyzer::startElement(const char *localname, const char *prefix,
                                  const char *uri, int nb_namespaces, const char **namespaces,
                                  int nb_attributes, int nb_defaulted, const char **attributes) {
}

void OdfContentHelperAnalyzer::endElement(const char *localname, const char *prefix,
                                const char *uri) {
}

void OdfContentHelperAnalyzer::characters(const char *data, uint32_t length) {
    assert(result != 0);

    result->addText(data,length);
}
