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
#include <pluginthroughanalyzer.h>

/* test class 1 */
class Analyzer1 : public jstreams::StreamThroughAnalyzer {
private:
    jstreams::Indexable* idx;
public:
    void setIndexable(jstreams::Indexable*i) {
        idx = i;
    }
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in) {
//        idx->setField("hi", "hi");
        return in;
    }
};

/* test class 2 */
class Analyzer2 : public jstreams::StreamThroughAnalyzer {
    void setIndexable(jstreams::Indexable*) {}
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in) {
        return in;
    }
};

REGISTER_THROUGHANALYZER(Analyzer1)
REGISTER_THROUGHANALYZER(Analyzer2)
