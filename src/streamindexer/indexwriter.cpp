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
#include "indexwriter.h"
#include "textutils.h"
using namespace jstreams;
using namespace std;
string
Indexable::getExtension() const {
    uint p1 = name.rfind('.');
    uint p2 = name.rfind('/');
    if (p1 != string::npos && (p2 == string::npos || p1 > p2)) {
        return name.substr(p1+1);
    }
    return "";
}
string
Indexable::getFileName() const {
    uint p = name.rfind('/');
    if (p != string::npos) {
        return name.substr(p+1);
    }
    return name;
}
void
Indexable::setField(const std::string& fieldname, const std::string& value) {
    // make sure only utf8 is stored
    if (!checkUtf8(fieldname)) {
        fprintf(stderr, "'%s' is not a UTF8 string\n", fieldname.c_str());
        return;
    }
    if (!checkUtf8(value)) {
        fprintf(stderr, "'%s' is not a UTF8 string\n", value.c_str());
        return;
    }
    if (strchr(fieldname.c_str(), '/') == 0) {
        writer->setField(this, fieldname, value);
    }
}
