/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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
#include "filtermanager.h"

#include "filters.h"
#include "../daemon/strigilogging.h"

#include "strigi_fnmatch.h"
#include <fstream>

using namespace std;

FilterManager::FilterManager()
{
    STRIGI_MUTEX_INIT(&m_rulesMutex);
    
    // won't index strigi configuration directory 
#ifdef WIN32
	m_strigidir.append( getenv("HOMEDRIVE") );
	m_strigidir.append( getenv("HOMEPATH") );
#else
    m_strigidir = getenv("HOME");
#endif
    m_strigidir += "/.strigi*";
}

FilterManager::~ FilterManager()
{
    clearRules();
    STRIGI_MUTEX_DESTROY (&m_rulesMutex);
}

void FilterManager::clearRules()
{
    STRIGI_MUTEX_LOCK (&m_rulesMutex);
    
    for (unsigned int i = 0; i < m_rules.size(); i++)
        delete m_rules[i];
    
    m_rules.clear();
    
    STRIGI_MUTEX_UNLOCK (&m_rulesMutex);
}

bool FilterManager::findMatch(const char* text, uint len)
{
    string t (text, len);
    return findMatch (t);
}

bool FilterManager::findMatch (string& text)
{
    STRIGI_MUTEX_LOCK (&m_rulesMutex);
    
    // check if text is related with strigi configuration directory
    int ret = fnmatch (m_strigidir.c_str(), text.c_str(), 0);
       
    if ((ret != FNM_NOMATCH) && (ret != 0))
        STRIGI_LOG_WARNING ("strigi.filtermanager.PathFilter", "error while applying pattern " + m_strigidir + "over text " + text)
    else if ( ret == 0)
    {
        STRIGI_LOG_DEBUG ("strigi.filtermanager.PathFilter", "Ignoring strigi configuration directory: " + m_strigidir)
        STRIGI_MUTEX_UNLOCK (&m_rulesMutex);
        return true;
    }
    
    for (vector<Filter*>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
    {
        Filter* filter = *iter;
        if (filter->match (text))
        {
            STRIGI_MUTEX_UNLOCK (&m_rulesMutex);
            return true;
        }
    }
   
    STRIGI_MUTEX_UNLOCK (&m_rulesMutex);

    //STRIGI_LOG_DEBUG ("strigi.filtermanager", text + " didn't match any pattern")
    return false;
}

multimap<int,string> FilterManager::getFilteringRules()
{
    STRIGI_MUTEX_LOCK (&m_rulesMutex);
    
    multimap<int,string> rules;
    
    for (vector<Filter*>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
    {
        Filter* filter = *iter;
        rules.insert(make_pair (int(filter->rtti()),filter->rule()));
    }

    STRIGI_MUTEX_UNLOCK (&m_rulesMutex);
    
    return rules;
}

void FilterManager::setFilteringRules(const multimap<int, string>& rules)
{
    clearRules();

    STRIGI_MUTEX_LOCK (&m_rulesMutex);
    
    multimap<int,string>::const_iterator iter;
    for (iter = rules.begin(); iter != rules.end(); iter++)
    {
        switch (iter->first)
        {
            case PathFilter::RTTI:
                m_rules.push_back (new PathFilter (iter->second));
                break;
            case PatternFilter::RTTI:
                m_rules.push_back (new PatternFilter (iter->second));
                break;
            default:
                STRIGI_LOG_ERROR ("strigi.filtermanager.setFilteringRules", "unknown rule RTTI")
        }
    }
    
    STRIGI_MUTEX_UNLOCK (&m_rulesMutex);
}
