/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "xmlindexwriter.h"
using namespace std;
using namespace Strigi;

void
XmlIndexWriter::initWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end(f.fields().end());
    for (i = f.fields().begin(); i != end; ++i) {
        Tag* tag = static_cast<Tag*>(i->second->writerData());
        if (tag) {
            tag->refcount++;
            continue;
        }
        tag = new Tag();
        tag->refcount = 1;
        const string s(i->first);
        const string& n = mapping.map(s);
        if (s == n) {
            tag->open = "  <value name='" + n + "'>";
            tag->close = "</value>\n";
        } else {
            tag->open = "  <" + n + '>';
            tag->close = "</" + n + ">\n";
        }
        i->second->setWriterData(tag);
    }
}
void
XmlIndexWriter::releaseWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end(f.fields().end());
    for (i = f.fields().begin(); i != end; ++i) {
        Tag* tag = static_cast<Tag*>(i->second->writerData());
        if (tag->refcount-- == 1) {
            //fprintf(stderr, "free for %s\n", i->second->key().c_str());
            delete tag;
            i->second->setWriterData(0);
        }
    }
}
