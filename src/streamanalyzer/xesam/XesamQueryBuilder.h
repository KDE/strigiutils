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

#ifndef _DIJON_XESAMQUERYBUILDER_H
#define _DIJON_XESAMQUERYBUILDER_H

#include <libxml/xmlreader.h>
#include <string>
#include <set>
#include <map>
#include <vector>

namespace Dijon
{
    typedef enum { And, Or } CollectorType;

    class Collector
    {
    public:
	Collector();
	Collector(CollectorType collector, bool negate, float boost);
	Collector(const Collector &other);
	~Collector();

	Collector& operator=(const Collector &other);

	CollectorType m_collector;
	bool m_negate;
	float m_boost;
    };

    typedef enum { None, Equals, Contains, LessThan, LessThanEquals, GreaterThan,
	GreaterThanEquals, StartsWith, InSet, FullText, RegExp, Proximity } SelectionType;

    typedef enum { String, Integer, Date, Boolean, Float } SimpleType;

    class Modifiers
    {
    public:
	Modifiers();
	Modifiers(const Modifiers &other);
	~Modifiers();

	Modifiers& operator=(const Modifiers &other);

	bool m_negate;
	float m_boost;
	bool m_phrase;
	bool m_caseSensitive;
	bool m_diacriticSensitive;
	int m_slack;
	bool m_ordered;
	bool m_enableStemming;
	std::string m_language;
	float m_fuzzy;
	int m_distance;
	bool m_wordBreak;
	bool m_fullTextFields;
    };

    /// Interface implemented by all query builders.
    class XesamQueryBuilder
    {
    public:
	/// Builds a query builder.
	XesamQueryBuilder();
	virtual ~XesamQueryBuilder();

	/// Called when the parser moves down into, or up into a collector block.
	virtual void set_collector(const Collector &collector);

	/// Called when the parser has read a userQuery element.
	virtual void on_user_query(const char *value);

	/// Called when the parser has read a query block.
	virtual void on_query(const char *type) = 0;

	/// Called when the parser has read a selection block.
	virtual void on_selection(SelectionType selection,
		const std::set<std::string> &field_names,
		const std::vector<std::string> &field_values,
		SimpleType field_type,
		const Modifiers &modifiers) = 0;

    protected:
	Collector m_collector;

    private:
	/// XesamQueryBuilder objects cannot be copied.
	XesamQueryBuilder(const XesamQueryBuilder &other);
	/// XesamQueryBuilder objects cannot be copied.
	XesamQueryBuilder& operator=(const XesamQueryBuilder &other);

    };
}

#endif // _DIJON_XESAMQUERYBUILDER_H
