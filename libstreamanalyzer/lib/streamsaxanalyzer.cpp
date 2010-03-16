/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias G. Pfeiffer <tgpfeiffer@web.de>
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
#include <strigi/streamsaxanalyzer.h>

using namespace Strigi;

class StreamSaxAnalyzer::Private {
public:
    Private() {
    }
};

StreamSaxAnalyzer::StreamSaxAnalyzer()
        :p(new Private()) {
}

StreamSaxAnalyzer::~StreamSaxAnalyzer() {
    delete p;
}

void
StreamSaxAnalyzer::endAnalysis(bool /*complete*/) {
}

void
StreamSaxAnalyzer::startElement(const char* localname, const char* prefix,
        const char* uri, int nb_namespaces, const char** namespaces,
        int nb_attributes,int nb_defaulted, const char** attributes) {
    (void)localname;
    (void)prefix;
    (void)uri;
    (void)nb_namespaces;
    (void)namespaces;
    (void)nb_attributes;
    (void)nb_defaulted;
    (void)attributes;
}

void
StreamSaxAnalyzer::endElement(const char* localname, const char* prefix,
        const char* uri) {
    (void)localname;
    (void)prefix;
    (void)uri;
}

void
StreamSaxAnalyzer::characters(const char* data, uint32_t length) {
    (void)data;
    (void)length;
}
