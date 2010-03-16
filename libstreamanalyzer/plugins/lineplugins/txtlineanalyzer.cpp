/* This file is part of Strigi Desktop Search
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

#include "txtlineanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

using namespace std;
using namespace Strigi;

// AnalyzerFactory
void
TxtLineAnalyzerFactory::registerFields(FieldRegister& reg) {
    totalLinesField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lineCount");
    totalWordsField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#wordCount");
    totalCharactersField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#characterCount");
    maxLineLengthField = reg.registerField(
        "http://strigi.sf.net/ontologies/homeless#maxLineLength");
//TODO: this field should receive a proper name. What is meant is whether
// the file uses uses unix (\n), dos (\r\n) or mac (\r) line endings
    formatField = reg.registerField(
        "http://strigi.sf.net/ontologies/homeless#lineEndingFormat");

    addField(totalLinesField);
    addField(totalWordsField);
    addField(totalCharactersField);
    addField(maxLineLengthField);
    addField(formatField);
}

// Analyzer
void
TxtLineAnalyzer::startAnalysis(AnalysisResult* i) {
    analysisResult = i;
    totalLines = 0;
    totalWords = 0;
    totalCharacters = 0;
    maxLineLength = 0;
    dos = false;
    ready = false;
}
void
TxtLineAnalyzer::handleLine(const char* data, uint32_t length) {
    bool inWord = false;

    totalLines++;
    totalCharacters += length;

    if (maxLineLength < length)
        maxLineLength = length;

    // instead of using regexp use this elementary solution
    for (unsigned int i = 0; i < length; i++) {
        bool spacer = true;
        if (isspace(data[i]) == 0)
            spacer = false;

        if (!spacer && !inWord) {// beginning of a word
            totalWords++;
            inWord = true;
        }
        //else if (!spacer && inWord) {/*inside word, do nothing*/}
        else if (spacer && inWord)
            inWord = false;
    }

    //TODO: by now it isn't possible to detect mac formatting
    //endline should be just '\r'. I don't know if it is still true with latest
    // versions of OSX, I have tried with tiger and I got a standard unix file.
    if (length > 0 && data[length-1] == '\r')
        dos = true;

}
void
TxtLineAnalyzer::endAnalysis(bool complete) {
    // we assume all cpp files must have includes
    if (complete) {
        analysisResult->addValue(factory->totalWordsField, (int32_t)totalWords);
        analysisResult->addValue(factory->totalCharactersField, (int32_t)totalCharacters);
        analysisResult->addValue(factory->totalLinesField, (int32_t)totalLines);
/* //FIXME: either get rid of this or replace with NIE equivalent
        analysisResult->addValue(factory->maxLineLengthField, (int32_t)maxLineLength);
        if (dos)
            analysisResult->addValue(factory->formatField, "DOS");
        else
            analysisResult->addValue(factory->formatField, "UNIX");
*/
    }
    ready = true;
}
bool
TxtLineAnalyzer::isReadyWithStream() {
    return ready;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamLineAnalyzerFactory*>
    streamLineAnalyzerFactories() const {
        list<StreamLineAnalyzerFactory*> af;
        af.push_back(new TxtLineAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
