/*
 *  Copyright 2007 Fabrice Colin
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iostream>

#include "XesamQueryBuilder.h"

using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

using namespace Dijon;

Collector::Collector() :
	m_collector(And),
	m_negate(false),
	m_boost(0.0)
{
}

Collector::Collector(CollectorType collector, bool negate, float boost) :
	m_collector(collector),
	m_negate(negate),
	m_boost(boost)
{
}

Collector::Collector(const Collector &other) :
	m_collector(other.m_collector),
	m_negate(other.m_negate),
	m_boost(other.m_boost)
{
}

Collector::~Collector()
{
}

Collector& Collector::operator=(const Collector &other)
{
	if (this != &other)
	{
		m_collector = other.m_collector;
		m_negate = other.m_negate;
		m_boost = other.m_boost;
	}

	return *this;
}

Modifiers::Modifiers() :
	m_negate(false),
	m_boost(0.0),
	m_phrase(true),
	m_caseSensitive(false),
	m_diacriticSensitive(true),
	m_slack(0),
	m_ordered(false),
	m_enableStemming(true),
	m_fuzzy(0.0),
	m_distance(0),
	m_wordBreak(false),
	m_fullTextFields(false)
{
}

Modifiers::Modifiers(const Modifiers &other) :
	m_negate(other.m_negate),
	m_boost(other.m_boost),
	m_phrase(other.m_phrase),
	m_caseSensitive(other.m_caseSensitive),
	m_diacriticSensitive(other.m_diacriticSensitive),
	m_slack(other.m_slack),
	m_ordered(other.m_ordered),
	m_enableStemming(other.m_enableStemming),
	m_language(other.m_language),
	m_fuzzy(other.m_fuzzy),
	m_distance(other.m_distance),
	m_wordBreak(other.m_wordBreak),
	m_fullTextFields(other.m_fullTextFields)
{
}

Modifiers::~Modifiers()
{
}

Modifiers& Modifiers::operator=(const Modifiers &other)
{
	if (this != &other)
	{
		m_negate = other.m_negate;
		m_boost = other.m_boost;
		m_phrase = other.m_phrase;
		m_caseSensitive = other.m_caseSensitive;
		m_diacriticSensitive = other.m_diacriticSensitive;
		m_slack = other.m_slack;
		m_ordered = other.m_ordered;
		m_enableStemming = other.m_enableStemming;
		m_language = other.m_language;
		m_fuzzy = other.m_fuzzy;
		m_distance = other.m_distance;
		m_wordBreak = other.m_wordBreak;
		m_fullTextFields = other.m_fullTextFields;
	}

	return *this;
}

XesamQueryBuilder::XesamQueryBuilder() :
	m_collector(And, false, 0.0)
{
}

XesamQueryBuilder::~XesamQueryBuilder()
{
}

void XesamQueryBuilder::set_collector(const Collector &collector)
{
	m_collector = collector;
}

void XesamQueryBuilder::on_user_query(const char *value)
{
#ifdef DEBUG
	cout << "XesamQueryBuilder::on_user_query: called" << endl;
#endif
    //TODO: IMPLEMENT!
/*	if (value != NULL)
	{
		XesamULParser ulParser;
		string xesamULQuery(value);

		ulParser.parse(xesamULQuery, *this);
	}*/
}

