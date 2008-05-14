/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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

#include "StrigiQueryBuilder.h"
#include "xesam_ul_driver.hh"
#include "strigilogging.h"
#include <iostream>
#include <sstream>

using std::string;
using std::stringstream;
using std::set;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

using namespace Dijon;

StrigiQueryBuilder::StrigiQueryBuilder() :
        XesamQueryBuilder(),
    m_fullQuery(),
    m_firstSelection(true),
    m_collectorChanged (false)
{
}

StrigiQueryBuilder::~StrigiQueryBuilder()
{
}

void StrigiQueryBuilder::set_collector(const Collector &collector)
{
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.set_collector", "called")

    XesamQueryBuilder::set_collector( collector);

    m_collectorChanged = true;
}

void StrigiQueryBuilder::on_query(const char *type)
{
    string msg = "called with type ";
    if (type != NULL)
        msg += type;
    else
        msg += "NULL";

    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_query", msg)
}

void StrigiQueryBuilder::on_selection(SelectionType selection,
                                      const set<string> &field_names,
                                      const vector<string> &field_values,
                                      SimpleType field_type,
                                      const Modifiers &modifiers)
{
    stringstream msg;
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", "---- START ---- ")
    msg << "called on " << field_names.size() << " fields";
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection",
            string("first selection: ") + (m_firstSelection ? "TRUE" : "FALSE"))

    Strigi::Query parsedQuery;
    msg << "selection type |";
    
    switch (selection)
    {
        case None:
            //TODO: Jos, is this right?
            msg << "None";
            break;
        case RegExp:
            parsedQuery.setType(Strigi::Query::RegExp);
            msg << "RegExp";
            break;
        case Equals:
            parsedQuery.setType(Strigi::Query::Equals);
            msg << "Equals";
            break;
        case Contains:
            parsedQuery.setType(Strigi::Query::Contains);
            msg << "Contains";
            break;
        case LessThan:
            parsedQuery.setType(Strigi::Query::LessThan);
            msg << "LessThan";
            break;
        case LessThanEquals:
            parsedQuery.setType(Strigi::Query::LessThanEquals);
            msg << "LessThanEquals";
            break;
        case GreaterThan:
            parsedQuery.setType(Strigi::Query::GreaterThan);
            msg << "GreaterThan";
            break;
        case GreaterThanEquals:
            parsedQuery.setType(Strigi::Query::GreaterThanEquals);
            msg << "GreaterThanEquals";
            break;
        case StartsWith:
            parsedQuery.setType(Strigi::Query::StartsWith);
            msg << "StartsWith";
            break;
        case FullText:
            parsedQuery.setType(Strigi::Query::FullText);
            msg << "FullText";
            break;
        case InSet:
            //TODO: hanlde!
            msg << "InSet, NOT YET HANLDED";
            break;
        case Proximity:
            //TODO: hanlde!
            msg << "Proximity, NOT YET HANLDED";
            break;
    }
    
    msg << "|";

    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())

    msg.str("");
    msg << "field names are ";

    for (set<string>::const_iterator iter = field_names.begin();
         iter != field_names.end(); iter++)
    {
        msg <<  " |" + *iter + "| ";

        parsedQuery.fields().push_back (*iter);
    }
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())

    parsedQuery.setBoost(modifiers.m_boost);
    parsedQuery.term().setCaseSensitive(modifiers.m_caseSensitive);
    parsedQuery.term().setDiacriticSensitive(modifiers.m_diacriticSensitive);
    parsedQuery.term().setProximityDistance(modifiers.m_distance); //TODO: check
    parsedQuery.term().setStemming(modifiers.m_enableStemming);
    parsedQuery.term().setFuzzy(modifiers.m_fuzzy);
    //TODO: handle modifiers.m_language
    //modifiers.m_negate already handled
    parsedQuery.term().setOrdered(modifiers.m_ordered);
    //TODO: handle modifiers.m_phrase
    parsedQuery.term().setSlack(modifiers.m_slack);

    msg.str("");
    msg << "there're " << field_values.size() << " field values";
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())

    msg.str("");
    msg << "field values are ";

    for (vector<string>::const_iterator valueIter = field_values.begin();
         valueIter != field_values.end(); ++valueIter)
    {
        string fieldValue(*valueIter);

        msg << " |" << *valueIter << "| ";

        parsedQuery.term().setValue( *valueIter);
    }

    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())

    parsedQuery.setNegate(modifiers.m_negate);

    msg.str("");
    msg << "collector is | "
        << (m_collector.m_collector == And ? "AND" : "OR" ) << " |";
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", msg.str())

    if (m_firstSelection == true)
    {
        m_fullQuery = parsedQuery;
        m_fullQuery.subQueries().clear();
        m_firstSelection = false;
    }
    else
    {
        Strigi::Query fullerQuery;
        
        if (m_collector.m_collector == And)
        {
            fullerQuery.setType (Strigi::Query::And);
            //fabrice handles negation here, we handle it some lines above
//             if (m_collector.m_negate == true)
//                 fullerQuery.setNegate( true);
        }
        else
            fullerQuery.setType (Strigi::Query::Or);

        if (m_collectorChanged)
        {
            fullerQuery.subQueries().push_back(m_fullQuery);
            fullerQuery.subQueries().push_back(parsedQuery);
            m_fullQuery = fullerQuery;
            m_collectorChanged = false;
        }
        else
            m_fullQuery.subQueries().push_back (parsedQuery);
    }

    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", " ---- END ---- ")

    /* More debugging infos, be careful: it's really verbose */
    /*
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", "query now "
                      + m_fullQuery.get_description())
    stringstream res;
    res << m_fullQuery;
    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.on_selection", "query now " +
                      res.str())
    */
}

Strigi::Query StrigiQueryBuilder::get_query(void) const
{
    stringstream result;
    result << "<results>\n";
    result << m_fullQuery;
    result << "</results>\n";

    STRIGI_LOG_DEBUG ("StrigiQueryBuilder.get_query", result.str())
    
    return m_fullQuery;
}

void StrigiQueryBuilder::on_user_query(const char *value)
{
  XesamUlDriver driver;
  Strigi::Query query;
    
  if (driver.parseString (value))
    m_fullQuery = *driver.query();
}
