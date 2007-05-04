/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 George Goldberg <grundleborg@gmail.com>
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

#include "strigiconfig.h"
#include "cpplineanalyzer.h"
#include "analysisresult.h"
#include "fieldtypes.h"

using namespace std;
using namespace Strigi;

// AnalyzerFactory
void
CppLineAnalyzerFactory::registerFields(FieldRegister& reg) {
    includeField = reg.registerField("content.depends", FieldRegister::stringType, 0, 0);
    classField = reg.registerField("source_code.describes_class", FieldRegister::stringType, 0, 0);
    codeLinesField = reg.registerField("source_code.stats.code_line_count", FieldRegister::integerType, 1, 0);
    commentLinesField = reg.registerField("source_code.stats.comment_line_count", FieldRegister::integerType, 1, 0);
    totalLinesField = reg.registerField("text.stats.line_count", FieldRegister::integerType, 1, 0);
// Include count not required. Include list length is easy to obtain.
//    includesField = reg.registerField("TODO_includes", FieldRegister::integerType, 1, 0);
}

// Analyzer
void
CppLineAnalyzer::startAnalysis(AnalysisResult* i) {
    analysisResult = i;
    ready = false;
    codeLines     = 0;
    commentLines  = 0;
    totalLines    = 0;
    includes      = 0;
    inComment = false;
}
void
CppLineAnalyzer::handleLine(const char* data, uint32_t length) {
    string line(data, length);

    totalLines++;

    if (line.find("/*") != string::npos) inComment = true;

    if (!inComment){

        codeLines++;

        size_t pos = line.find_first_of("//");
        if (pos != string::npos) commentLines++;

        //TODO Add code here for counting strings.
        //Look for included files.
        size_t pos1 = line.find("#include",0);
        if(pos1 != string::npos){
            string include1 = line.substr(8+pos1,line.size());
            size_t pos2 = include1.find("<",0);
            size_t pos3 = include1.find(">",0);
            if((pos2 != string::npos) && (pos3 != string::npos)){
                analysisResult->addValue(factory->includeField, include1.substr(1+pos2,((pos3-1)-pos2)));
                includes++;
            }
            size_t pos4 = include1.find("\"",0);
            size_t pos5 = include1.find("\"", pos4+1);
            if((pos4 != string::npos) && (pos5 != string::npos)){
                analysisResult->addValue(factory->includeField, include1.substr(1+pos4,((pos5-1)-pos4)));
                includes++;
            }
        }

    }
    else
        commentLines++;

    if (line.find("*/") != string::npos) inComment = false;
}
void
CppLineAnalyzer::endAnalysis() {
    // we assume all cpp files must have includes
    if (includes) {
        analysisResult->addValue(factory->codeLinesField, codeLines);
        analysisResult->addValue(factory->commentLinesField, commentLines);
        analysisResult->addValue(factory->totalLinesField, totalLines);
//        analysisResult->addValue(factory->includesField, includes);
    }
    ready = true;
}
bool
CppLineAnalyzer::isReadyWithStream() {
    return ready;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamLineAnalyzerFactory*>
    streamLineAnalyzerFactories() const {
        list<StreamLineAnalyzerFactory*> af;
        af.push_back(new CppLineAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
