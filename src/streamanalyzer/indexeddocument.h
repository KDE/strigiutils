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
#ifndef STRIGI_INDEXEDDOCUMENT_H
#define STRIGI_INDEXEDDOCUMENT_H

#include <map>
#include <string>
#ifdef HAVE_STDINT_H
 #include <stdint.h>
#endif
#include "strigi/strigiconfig.h"

namespace Strigi {

class IndexedDocument {
public:
    IndexedDocument() :score(0), size(-1), mtime(0) {}
	// FIXME: trueg: this is not a uri but a URL or even just a path.
    std::string uri;
    float score;
    std::string fragment;
    std::string mimetype;
    std::string sha1;
    int64_t size;
    uint64_t mtime;
    // trueg: is this intended to be restricted to strings? There could
    // be other types, right?
    std::multimap<std::string, std::string> properties;
};

}

#endif
