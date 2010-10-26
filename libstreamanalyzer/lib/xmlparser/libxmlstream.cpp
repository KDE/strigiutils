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
#include <cstring>
#include <stack>
#include <libxml/parser.h>
using namespace std;

class SimpleNodeParser {
    int depth;
    stack<SimpleNode*> nodes;
    xmlSAXHandler handler;
    static void charactersSAXFunc(void* ctx, const xmlChar * ch, int len);
    static void errorSAXFunc(void* /*ctx*/, const char * /*msg*/, ...) {}
    static void startElementSAXFunc(void * ctx, const xmlChar * name,
        const xmlChar ** atts);
    static void endElementSAXFunc(void * ctx, const xmlChar * name);
public:
    SimpleNodeParser() {
        memset(&handler, 0, sizeof(xmlSAXHandler));
        handler.characters = charactersSAXFunc;
        handler.error = errorSAXFunc;
        handler.startElement = startElementSAXFunc;
        handler.endElement = endElementSAXFunc;
    }
    ~SimpleNodeParser() {
    }
    void parse(const string& xml, SimpleNode& node);
};
void
SimpleNodeParser::parse(const string& xml, SimpleNode& node) {
    depth = 0;
    nodes.push(&node);
    if (xmlSAXUserParseMemory(&handler, this, xml.c_str(), (int)xml.length())) {
        printf("parsing error: %s\n", "");
        // handle the error unless it is a tag mismatch in html
//        errorstring = XML_ErrorString(e);
//        error = stop = true;
//        wellformed = false;
    }
}
void
SimpleNodeParser::charactersSAXFunc(void* ctx, const xmlChar* ch, int len) {
    SimpleNodeParser* p = static_cast<SimpleNodeParser*>(ctx);
    p->nodes.top()->text.append((const char*)ch, len);
}
void
SimpleNodeParser::startElementSAXFunc(void* ctx, const xmlChar* name,
        const xmlChar** atts) {
    SimpleNodeParser* p = static_cast<SimpleNodeParser*>(ctx);
    SimpleNode* node = p->nodes.top();
    //printf("%s %i\n", name, node->nodes.size());

    // if this is not the root node, add it to the stack and to the parent node
    if (p->depth > 0) {

        SimpleNode emptynode;
        SimpleNode* prev = 0;
        if (!node->nodes.empty()) {
            prev = &node->nodes.back();
        }
        node->nodes.push_back(emptynode);
        SimpleNode* cnode = &*(node->nodes.rbegin());
        cnode->parent = node;
        if (prev) {
            prev->next = cnode;
        }

        node = cnode;
        p->nodes.push(node);
    }

    node->tagname = (const char*)name;
    while ((atts != NULL) && (*atts)) {
        node->atts[(const char*)*atts] = (const char*)*(atts+1);
        atts += 2;
    }
    p->depth++;
}
void
SimpleNodeParser::endElementSAXFunc(void* ctx, const xmlChar* name) {
    SimpleNodeParser* p = static_cast<SimpleNodeParser*>(ctx);
    p->nodes.pop();
    p->depth--;
}
SimpleNode::SimpleNode(const string& xml) :parent(0), next(0) {
    SimpleNodeParser parser;
    parser.parse(xml, *this);
}

