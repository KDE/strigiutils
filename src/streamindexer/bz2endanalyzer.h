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
#ifndef BZ2ENDANALYZER
#define BZ2ENDANALYZER

#include "streamendanalyzer.h"
#include "inputstream.h"

class BZ2EndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(jstreams::Indexable& idx, jstreams::InputStream* in);
    const char* getName() const { return "BZ2EndAnalyzer"; }
};

class BZ2EndAnalyzerFactory : public jstreams::StreamEndAnalyzerFactory {
public:
    const char* getName() const {
        return "BZ2EndAnalyzer";
    }
    jstreams::StreamEndAnalyzer* newInstance() const {
        return new BZ2EndAnalyzer();
    }
    bool analyzesSubStreams() const { return true; }
    void registerFields(jstreams::FieldRegister&);
};

#endif
