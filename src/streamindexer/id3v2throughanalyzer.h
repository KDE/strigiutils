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
#ifndef IDV32THROUGHANALYZER_H
#define IDV32THROUGHANALYZER_H

#include "streamthroughanalyzer.h"

// id3v2 according to http://www.id3.org/id3v2.4.0-structure.txt

class ID3V2ThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    jstreams::Indexable* indexable;
public:
    ID3V2ThroughAnalyzer();
    ~ID3V2ThroughAnalyzer();
    void setIndexable(jstreams::Indexable*);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
    bool isReadyWithStream();
};

#endif
