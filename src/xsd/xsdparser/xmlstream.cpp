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
#include "xmlstream.h"
#include <map>
#include <list>
#include <vector>
#include <stack>
#include <stdlib.h>
using namespace std;

class XMLStream::Private {
private:
public:
    const SimpleNode root;
    const SimpleNode* activeNode;
    Private(const string& xml) :root(xml), activeNode(&root) {
    }
};
XMLStream::XMLStream(const string& xml) {
    p = new Private(xml);
}
XMLStream::~XMLStream() {
    delete p;
}
void
XMLStream::setFromAttribute(bool& v, const char* name) {
    map<string, string>::const_iterator i = p->activeNode->atts.find(name);
    if (i != p->activeNode->atts.end()
            && (i->second == "true" || i->second == "1")) {
        v = true;
    } else {
        v = false;
    }
}
void
XMLStream::setFromAttribute(int& v, const char* name) {
    map<string, string>::const_iterator i = p->activeNode->atts.find(name);
    if (i == p->activeNode->atts.end()) {
        v = 0;
    } else {
        v = atoi(i->second.c_str());
    }
}
void
XMLStream::setFromAttribute(string& v, const char* name) {
    map<string, string>::const_iterator i = p->activeNode->atts.find(name);
    if (i == p->activeNode->atts.end()) {
        v = "";
    } else {
        v = i->second;
    }
}
const string&
XMLStream::getTagName() const {
    return p->activeNode->tagname;
}
XMLStream&
operator>>(XMLStream& in, bool& e) {
    e = in.currentNode().text == "true" || in.currentNode().text == "1";
    return in;
}
XMLStream&
operator>>(XMLStream& in, int& e) {
    e = atoi(in.currentNode().text.c_str());
    return in;
}
XMLStream&
operator>>(XMLStream& in, string& e) {
    e = in.currentNode().text;
    return in;
}
const SimpleNode&
XMLStream::currentNode() const {
    return *p->activeNode;
}
const SimpleNode*
XMLStream::firstChild() const {
    const SimpleNode* n = 0;
    if (!p->activeNode->nodes.empty()) {
        n = p->activeNode = &*p->activeNode->nodes.begin();
    }
    return n;
}
const SimpleNode*
XMLStream::nextSibling() const {
    const SimpleNode* n = p->activeNode->next;
    if (n) {
        p->activeNode = n;
    }
    return n;
}
const SimpleNode*
XMLStream::parentNode() const {
    const SimpleNode* n = p->activeNode->parent;
    if (n) {
        p->activeNode = n;
    }
    return n;
}
