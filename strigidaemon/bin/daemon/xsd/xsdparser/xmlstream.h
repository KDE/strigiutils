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
#ifndef XMLSTREAM_H
#define XMLSTREAM_H

#include <string>
#include <map>
#include <list>
// Although ostream isn't used *here*, other generated code
// which #includes this file does use std::ostream (while
// it doesn't #include <ostream> itself) so pull the header
// in here.
#include <ostream>

class SimpleNode;
class XMLStream {
private:
    class Private;
    Private* p;
public:
    XMLStream(const std::string& xml);
    ~XMLStream();
    void setFromAttribute(bool&, const char*);
    void setFromAttribute(int&, const char*);
    void setFromAttribute(std::string&, const char*);
    const std::string& getTagName() const;
    /**
     * The node at which the XMLStream is currently positioned.
     **/
    const SimpleNode& currentNode() const;
    /**
     * Moves the XMLStream to the first visible child of the current node, and
     * returns the new node. If the current node has no visible children,
     * returns null, and retains the current node.
     **/
    const SimpleNode* firstChild() const;
    /**
     * Moves the XMLStream to the next sibling of the current node, and
     * returns the new node. If the current node has no visible next sibling,
     * returns null, and retains the current node.
     **/
    const SimpleNode* nextSibling() const;
    /**
     * Moves to and returns the closest visible ancestor node of the current
     * node. If the search for parentNode attempts to step upward from the
     * XMLStream's root node, or if it fails to find a visible ancestor node,
     * this method retains the current position and returns null.
     **/
    const SimpleNode* parentNode() const;
};

XMLStream& operator>>(XMLStream& in, bool& e);
XMLStream& operator>>(XMLStream& in, int& e);
XMLStream& operator>>(XMLStream& in, std::string& e);

class SimpleNode {
friend class SimpleNodeParser;
private:
    SimpleNode() :parent(0), next(0) {}
public:
    const SimpleNode* parent;
    const SimpleNode* next;
    std::string tagname;
    std::map<std::string, std::string> atts;
    std::list<SimpleNode> nodes;
    std::string text;

    SimpleNode(const std::string& xml);
};


#endif
