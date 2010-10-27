/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2010 Klarälvdalens Datakonsult AB,
 *     a KDAB Group company, info@kdab.net,
 *     author Tobias Koenig <tokoe@kde.org>
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

#include "xesamparser.h"

#include "strigi/query.h"
#include "xmlparser/xmlstream.h"

#include <iostream>

using namespace Strigi;

XesamParser::XesamParser()
  : m_xmlStream(0)
{
}

XesamParser::~XesamParser()
{
    delete m_xmlStream;
}

std::string XesamParser::errorMessage() const
{
    return m_errorMessage;
}

bool XesamParser::buildQuery(const std::string& queryStr, Query &query)
{
    m_xmlStream = new XMLStream(queryStr);

    const SimpleNode *rootNode = &m_xmlStream->currentNode();
    if (!rootNode) {
        m_errorMessage = "Root node not available";
        return false;
    }

    if (m_xmlStream->getTagName() != "request") {
        m_errorMessage = "Invalid root node: " + m_xmlStream->getTagName();
        return false;
    }

    const SimpleNode *queryNode = m_xmlStream->firstChild();
    if (!queryNode) {
        m_errorMessage = "Query node not available";
        return false;
    }

    if (m_xmlStream->getTagName() == "query") {
        if (!parseQuery(query)) {
            query = Query();
            return false;
        }
    } else if (m_xmlStream->getTagName() == "userQuery") {
        // should be handled somewhere else
        return false;
    } else {
        m_errorMessage = "Invalid query node: " + m_xmlStream->getTagName();
        return false;
    }

    return true;
}

bool XesamParser::parseQuery(Query &query)
{
    int childCount = 0;
    const SimpleNode *node = m_xmlStream->firstChild();
    while (node) {
        childCount++;
        if (childCount == 2) {
            m_errorMessage = "Query can have only one child node";
            return false;
        }

        if (m_xmlStream->getTagName() == "equals") {
            if (!parseEquals(query))
                return false;
        } else if (m_xmlStream->getTagName() == "contains") {
            if (!parseContains(query))
                return false;
        } else if (m_xmlStream->getTagName() == "lessThan") {
            if (!parseLessThan(query))
                return false;
        } else if (m_xmlStream->getTagName() == "lessThanEquals") {
            if (!parseLessThanEquals(query))
                return false;
        } else if (m_xmlStream->getTagName() == "greaterThan") {
            if (!parseGreaterThan(query))
                return false;
        } else if (m_xmlStream->getTagName() == "greaterThanEquals") {
            if (!parseGreaterThanEquals(query))
                return false;
        } else if (m_xmlStream->getTagName() == "startsWith") {
            if (!parseStartsWith(query))
                return false;
        } else if (m_xmlStream->getTagName() == "inSet") {
            if (!parseInSet(query))
                return false;
        } else if (m_xmlStream->getTagName() == "fullText") {
            if (!parseFullText(query))
                return false;
        } else if (m_xmlStream->getTagName() == "and") {
            if (!parseAnd(query))
                return false;
        } else if (m_xmlStream->getTagName() == "or") {
            if (!parseOr(query))
                return false;
        } else {
            m_errorMessage = "Unknown tag: " + m_xmlStream->getTagName();
            return false;
        }

        node = m_xmlStream->nextSibling();
    }

    m_xmlStream->parentNode();
    return true;
}

bool XesamParser::parseSelectorClause(Query &query, Query::Type type)
{
    query.setType(type);
    query.subQueries().clear();

    bool negate = false;
    m_xmlStream->setFromAttribute(negate, "negate");
    query.setNegate(negate);

    const SimpleNode *node = m_xmlStream->firstChild();
    while (node) {
        if (m_xmlStream->getTagName() == "field") {
            std::string fieldName;
            m_xmlStream->setFromAttribute(fieldName, "name");
            query.fields().push_back(fieldName);
        } else if (m_xmlStream->getTagName() == "fullTextFields") {
        } else if (m_xmlStream->getTagName() == "string") {
            if (!parseString(query))
              return false;
        } else if (m_xmlStream->getTagName() == "integer") {
            if (!parseInteger(query))
              return false;
        } else if (m_xmlStream->getTagName() == "date") {
            if (!parseDate(query))
              return false;
        } else if (m_xmlStream->getTagName() == "float") {
            if (!parseFloat(query))
              return false;
        } else if (m_xmlStream->getTagName() == "boolean") {
            if (!parseBoolean(query))
              return false;
        } else {
            m_errorMessage = "Unknown tag: " + m_xmlStream->getTagName();
            return false;
        }

        node = m_xmlStream->nextSibling();
    }

    m_xmlStream->parentNode();
    return true;
}

bool XesamParser::parseEquals(Query &query)
{
    return parseSelectorClause(query, Query::Equals);
}

bool XesamParser::parseContains(Query &query)
{
    return parseSelectorClause(query, Query::Contains);
}

bool XesamParser::parseLessThan(Query &query)
{
    return parseSelectorClause(query, Query::LessThan);
}

bool XesamParser::parseLessThanEquals(Query &query)
{
    return parseSelectorClause(query, Query::LessThanEquals);
}

bool XesamParser::parseGreaterThan(Query &query)
{
    return parseSelectorClause(query, Query::GreaterThan);
}

bool XesamParser::parseGreaterThanEquals(Query &query)
{
    return parseSelectorClause(query, Query::GreaterThanEquals);
}

bool XesamParser::parseStartsWith(Query &query)
{
    return parseSelectorClause(query, Query::StartsWith);
}

bool XesamParser::parseInSet(Query &query)
{
    query.setType(Query::Or);
    query.subQueries().clear();

    bool negate = false;
    m_xmlStream->setFromAttribute(negate, "negate");
    query.setNegate(negate);

    std::vector<std::string> fields;
    std::vector<Term> terms;
    const SimpleNode *node = m_xmlStream->firstChild();
    while (node) {
        Query dummyQuery;
        if (m_xmlStream->getTagName() == "field") {
            std::string fieldName;
            m_xmlStream->setFromAttribute(fieldName, "name");
            fields.push_back(fieldName);
        } else if (m_xmlStream->getTagName() == "fullTextFields") {
        } else if (m_xmlStream->getTagName() == "string") {
            if (!parseString(dummyQuery))
              return false;
            terms.push_back(dummyQuery.term());
        } else if (m_xmlStream->getTagName() == "integer") {
            if (!parseInteger(dummyQuery))
              return false;
            terms.push_back(dummyQuery.term());
        } else if (m_xmlStream->getTagName() == "date") {
            if (!parseDate(dummyQuery))
              return false;
            terms.push_back(dummyQuery.term());
        } else if (m_xmlStream->getTagName() == "float") {
            if (!parseFloat(dummyQuery))
              return false;
            terms.push_back(dummyQuery.term());
        } else if (m_xmlStream->getTagName() == "boolean") {
            if (!parseBoolean(dummyQuery))
              return false;
            terms.push_back(dummyQuery.term());
        } else {
            m_errorMessage = "Unknown tag: " + m_xmlStream->getTagName();
            return false;
        }

        node = m_xmlStream->nextSibling();
    }

    std::vector<Term>::const_iterator it = terms.begin();
    std::vector<Term>::const_iterator endId = terms.end();
    for (; it != endId; ++it) {
        Query subQuery;
        subQuery.setType(Query::Equals);
        subQuery.setTerm(*it);
        subQuery.fields() = fields;

        query.subQueries().push_back(subQuery);
    }

    // normalize
    if (query.subQueries().size() == 1) {
        Query q = query.subQueries()[0];
        query = q;
    }

    m_xmlStream->parentNode();
    return true;
}

bool XesamParser::parseFullText(Query &query)
{
    return parseSelectorClause(query, Query::FullText);
}

bool XesamParser::parseCollectorClause(Query &query, Query::Type type)
{
    query.setType(type);
    query.subQueries().clear();

    const SimpleNode *node = m_xmlStream->firstChild();
    while (node) {
        Query subQuery;
        if (m_xmlStream->getTagName() == "equals") {
            if (!parseEquals(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "contains") {
            if (!parseContains(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "lessThan") {
            if (!parseLessThan(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "lessThanEquals") {
            if (!parseLessThanEquals(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "greaterThan") {
            if (!parseGreaterThan(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "greaterThanEquals") {
            if (!parseGreaterThanEquals(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "startsWith") {
            if (!parseStartsWith(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "inSet") {
            if (!parseInSet(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "fullText") {
            if (!parseFullText(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "and") {
            if (!parseAnd(subQuery))
                return false;
        } else if (m_xmlStream->getTagName() == "or") {
            if (!parseOr(subQuery))
                return false;
        } else {
            m_errorMessage = "Unknown tag: " + m_xmlStream->getTagName();
            return false;
        }
        query.subQueries().push_back(subQuery);

        node = m_xmlStream->nextSibling();
    }

    // normalize
    if (query.subQueries().size() == 1) {
        Query q = query.subQueries()[0];
        query = q;
    }

    m_xmlStream->parentNode();
    return true;
}

bool XesamParser::parseAnd(Query &query)
{
    return parseCollectorClause(query, Query::And);
}

bool XesamParser::parseOr(Query &query)
{
    return parseCollectorClause(query, Query::Or);
}

bool XesamParser::parseString(Query &query)
{
    query.term().setValue(m_xmlStream->currentNode().text);
    return true;
}

bool XesamParser::parseInteger(Query &query)
{
    query.term().setValue(m_xmlStream->currentNode().text);
    return true;
}

bool XesamParser::parseDate(Query &query)
{
    query.term().setValue(m_xmlStream->currentNode().text);
    return true;
}

bool XesamParser::parseFloat(Query &query)
{
    query.term().setValue(m_xmlStream->currentNode().text);
    return true;
}

bool XesamParser::parseBoolean(Query &query)
{
    query.term().setValue(m_xmlStream->currentNode().text);
    return true;
}
