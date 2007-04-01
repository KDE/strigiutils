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
#include "jstreamsconfig.h"
#include "digesteventanalyzer.h"
#include "streambase.h"
#include "analysisresult.h"
#include "fieldtypes.h"
using namespace std;
using namespace Strigi;

DigestEventAnalyzer::DigestEventAnalyzer(FieldRegister& reg) {
    analysisresult = 0;
    shafield = reg.registerField("sha1", FieldRegister::binaryType, 1, 0);
}
DigestEventAnalyzer::~DigestEventAnalyzer() {
}
void
DigestEventAnalyzer::startAnalysis(AnalysisResult* ar) {
    analysisresult = ar;
}
void
DigestEventAnalyzer::handleData(const char* data, uint32_t length) {
}
void
DigestEventAnalyzer::endAnalysis() {
}
bool
DigestEventAnalyzer::isReadyWithStream() {
    return false;
}
