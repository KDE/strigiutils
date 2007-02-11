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
#include "digestthroughanalyzer.h"
#include "digestinputstream.h"
#include "inputstream.h"
#include "indexable.h"
#include "fieldtypes.h"
using namespace std;
using namespace jstreams;

cnstr DigestThroughAnalyzer::shafieldname("sha1");

DigestThroughAnalyzer::DigestThroughAnalyzer(FieldRegister& reg) {
    stream = 0;
    indexable = 0;
    shafield = reg.registerField(shafieldname, FieldRegister::binaryType, 1, 0);
}
DigestThroughAnalyzer::~DigestThroughAnalyzer() {
    if (stream) {
        delete stream;
    }
}
InputStream *
DigestThroughAnalyzer::connectInputStream(InputStream *in) {
    if (stream) {
        delete stream;
    }
    stream = new DigestInputStream(in);
    return stream;
}
void
DigestThroughAnalyzer::setIndexable(jstreams::Indexable* idx) {
    if (indexable && stream) { // && stream->getStatus() == Eof) {
        indexable->setField(shafield, stream->getDigestString());
//        printf("%s: %s\n", indexable->getName().c_str(), stream->getDigestString().c_str());
    }

    indexable = idx;
}
bool
DigestThroughAnalyzer::isReadyWithStream() {
    return stream->getStatus() != Ok;
}
