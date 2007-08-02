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

#include <iostream>

#include "StrigiQueryBuilder.h"

using std::string;
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
#ifdef DEBUG
    cout << "StrigiQueryBuilder::set_collector\n";
#endif

    XesamQueryBuilder::set_collector( collector);

    m_collectorChanged = true;
}

void StrigiQueryBuilder::on_query(const char *type)
{
#ifdef DEBUG
    cout << "StrigiQueryBuilder::on_query: called";
    if (type != NULL)
    {
        cout << " with " << type;
    }
    cout << endl;
#endif
}

void StrigiQueryBuilder::on_selection(SelectionType selection,
                                      const set<string> &field_names,
                                      const vector<string> &field_values,
                                      SimpleType field_type,
                                      const Modifiers &modifiers)
{
#ifdef DEBUG
    cout << " ---- START ---- " << endl;
    cout << "StrigiQueryBuilder::on_selection: called on "
            << field_names.size() << " fields" << endl;
    cout << "first selection: " << (m_firstSelection ? "TRUE" : "FALSE")<< endl;
#endif

    Strigi::Query parsedQuery;

#ifdef DEBUG
    cout << "StrigiQueryBuilder::on_selection: selection type\n |";
#endif
    switch (selection)
    {
        case None:
            //TODO: Jos, is this right?
#ifdef DEBUG
            cout << "None";
#endif
            break;
        case RegExp:
            parsedQuery.setType(Strigi::Query::RegExp);
#ifdef DEBUG
            cout << "RegExp";
#endif
            break;
        case Equals:
            parsedQuery.setType(Strigi::Query::Equals);
#ifdef DEBUG
            cout << "Equals";
#endif
            break;
        case Contains:
            parsedQuery.setType(Strigi::Query::Contains);
#ifdef DEBUG
            cout << "Contains";
#endif
            break;
        case LessThan:
            parsedQuery.setType(Strigi::Query::LessThan);
#ifdef DEBUG
            cout << "LessThan";
#endif
            break;
        case LessThanEquals:
            parsedQuery.setType(Strigi::Query::LessThanEquals);
#ifdef DEBUG
            cout << "LessThanEquals";
#endif
            break;
        case GreaterThan:
            parsedQuery.setType(Strigi::Query::GreaterThan);
#ifdef DEBUG
            cout << "GreaterThan";
#endif
            break;
        case GreaterThanEquals:
            parsedQuery.setType(Strigi::Query::GreaterThanEquals);
#ifdef DEBUG
            cout << "GreaterThanEquals";
#endif
            break;
        case StartsWith:
            parsedQuery.setType(Strigi::Query::StartsWith);
#ifdef DEBUG
            cout << "StartsWith";
#endif
            break;
        case FullText:
            parsedQuery.setType(Strigi::Query::FullText);
#ifdef DEBUG
            cout << "FullText";
#endif
            break;
        case InSet:
            //TODO: hanlde!
#ifdef DEBUG
            cout << "InSet, NOT YET HANLDED";
#endif
            break;
        case Proximity:
            //TODO: hanlde!
#ifdef DEBUG
            cout << "Proximity, NOT YET HANLDED";
#endif
            break;
    }
#ifdef DEBUG
    cout << "|" << endl;
#endif


#ifdef DEBUG
    cout << "StrigiQueryBuilder::on_selection: field names are " << endl;;
#endif

    for (set<string>::iterator iter = field_names.begin();
         iter != field_names.end(); iter++)
    {
#ifdef DEBUG
        cout << " |" << *iter << "| ";
#endif
        parsedQuery.fields().push_back (*iter);
    }
#ifdef DEBUG
    cout << endl;
#endif

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

#ifdef DEBUG
    cout << "StrigiQueryBuilder::on_selection: there're " <<
            field_values.size() << " field values" << endl;
    cout << "StrigiQueryBuilder::on_selection: field values are " << endl;
#endif

    for (vector<string>::const_iterator valueIter = field_values.begin();
         valueIter != field_values.end(); ++valueIter)
    {
        string fieldValue(*valueIter);

#ifdef DEBUG
        cout << " |" << *valueIter << "| ";
#endif

        parsedQuery.term().setValue( *valueIter);
    }
#ifdef DEBUG
    cout << endl;
#endif

    parsedQuery.setNegate(modifiers.m_negate);
    
#ifdef DEBUG
    cout << "StrigiQueryBuilder::on_selection: collector is\n| "
         << (m_collector.m_collector == And ? "AND" : "OR" ) << " |" << endl;
#endif
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

#ifdef DEBUG
    cout << " ---- END ---- " << endl;
#endif

    /* More debugging infos, be careful: it's really verbose */
/*#ifdef DEBUG
//     cout << "StrigiQueryBuilder::on_selection: query now " << m_fullQuery.get_description() << endl;
    cout << endl << endl << "StrigiQueryBuilder::on_selection: query now " << endl;
    cout << m_fullQuery;
    cout << endl << endl;
#endif*/
}

Strigi::Query StrigiQueryBuilder::get_query(void) const
{
#ifdef DEBUG
    cout << "<results>" << endl;
    cout << m_fullQuery;
    cout << "</results>" << endl;
#endif
    return m_fullQuery;
}
