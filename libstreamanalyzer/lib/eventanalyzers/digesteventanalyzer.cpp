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
#include "digesteventanalyzer.h"
#include <strigi/streambase.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
using namespace std;
using namespace Strigi;

DigestEventAnalyzer::DigestEventAnalyzer(const DigestEventAnalyzerFactory* f)
        :factory(f) {
//fprintf(stderr, "NEW\n");
    analysisresult = 0;
}
DigestEventAnalyzer::~DigestEventAnalyzer() {
}
void
DigestEventAnalyzer::startAnalysis(AnalysisResult* ar) {
//fprintf(stderr, "start NEW\n");
    analysisresult = ar;
    sha1.reset();
}
void
DigestEventAnalyzer::handleData(const char* data, uint32_t length) {
//fprintf(stderr, "process %i\n", length);
    sha1.process(data, length);
}
void
DigestEventAnalyzer::endAnalysis(bool complete) {
//fprintf(stderr, "end\n");
    const unsigned char* digest = sha1.hash();
    char d[41];
    for (int i = 0; i < 20; i++) {
        sprintf(d+2*i, "%02x", *digest++);
    }
    string hashUri = analysisresult->newAnonymousUri();
    analysisresult->addValue(factory->shafield, hashUri);
    analysisresult->addTriplet(hashUri,
			       "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
			       "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash");
    analysisresult->addTriplet(hashUri,
			       "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm",
			       "SHA1");
    analysisresult->addTriplet(hashUri,
			       "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue",
			       string(d,40));
    analysisresult = 0;
}
bool
DigestEventAnalyzer::isReadyWithStream() {
    return false;
}
void
DigestEventAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    shafield = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash");
    addField(shafield);
}
