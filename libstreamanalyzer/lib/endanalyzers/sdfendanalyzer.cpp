/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#include "sdfendanalyzer.h"
#include <strigi/sdfinputstream.h>
#include <strigi/fieldtypes.h>
#include <iostream>
using namespace Strigi;
using namespace std;

void
SdfEndAnalyzerFactory::registerFields(FieldRegister& r) {
    moleculeCountField = r.registerField(
        "http://rdf.openmolecules.net/0.9#moleculeCount");
    addField(moleculeCountField);
}

bool
SdfEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return SdfInputStream::checkHeader(header, headersize);
}

signed char
SdfEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if (!in)
        return -1;

    SdfInputStream sdf(in);
    InputStream *s = sdf.nextEntry();
    if (sdf.status() == Error) {
        m_error = sdf.error();
        return -1;
    }
    
    // explicit exit if not an SDfile (possibly MDL, since header matched)
    if (!s)
	return 0;
    
    int moleculeCount = 0;
    while (s) {
	moleculeCount++;
	std::string file = sdf.entryInfo().filename;
	idx.indexChild(file, idx.mTime(), s);
	
	s = sdf.nextEntry();
    }
    if (moleculeCount)
	idx.addValue(factory->moleculeCountField, moleculeCount);

    if (sdf.status() == Error) {
        m_error = sdf.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    return 0;
}

