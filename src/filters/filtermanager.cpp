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

#include "filtermanager.h"

#include "filters.h"
#include "../daemon/strigilogging.h"

#include <fnmatch.h>
#include <fstream>

using namespace std;

FilterManager::FilterManager()
{
    pthread_mutex_init(&m_mutex, NULL);
    
    // won't index strigi configuration directory 
    m_strigidir = getenv("HOME");
    m_strigidir += "/.strigi*";
}

FilterManager::~ FilterManager()
{
    saveFilter();
    clearRules();
    pthread_mutex_destroy (&m_mutex);
}

void FilterManager::clearRules()
{
    for (unsigned int i = 0; i < m_rules.size(); i++)
        delete m_rules[i];
    
    m_rules.clear();
}

void FilterManager::setConfFile (string& patternRules, string& pathRules)
{
    m_patternFile = patternRules;
    m_pathFile = pathRules;
   
    // clear old rules
    // TODO: remove when weì'll have a single configuration file
    clearRules();
    
    loadFilter(patternRules, PatternFilter::RTTI);
    loadFilter(pathRules, PathFilter::RTTI);
}

void FilterManager::loadFilter(string& file, unsigned int filterRTTI)
{
    fstream confFile;
    string rule;
    char buffer [500];
   
    confFile.open(file.c_str(), ios::in);
   
    // clear old rules
    // TODO: restore when weì'll have a single configuration file
    //clearRules();
    
    if (confFile.is_open())
    {
        pthread_mutex_lock (&m_mutex);
       
        // read filter rules
        while (!confFile.eof())
        {
            confFile.getline(buffer, 500);
            rule = buffer;
            
            if (rule.size() > 0)
            {
                switch (filterRTTI)
                {
                    case PathFilter::RTTI:
                        m_rules.push_back (new PathFilter (string(buffer)));
                        STRIGI_LOG_DEBUG ("strigi.filtermanager.loadFilter", "added path filter: |" + string(buffer) + "|")
                        break;
                    case PatternFilter::RTTI:
                        m_rules.push_back (new PatternFilter (string(buffer)));
                        STRIGI_LOG_DEBUG ("strigi.filtermanager.loadFilter", "added pattern filter: |" + string(buffer) + "|")
                        break;
                    default:
                        STRIGI_LOG_ERROR ("strigi.filtermanager.loadFilter", "unknown rule RTTI")
                        break;
                }
            }
        }
       
        pthread_mutex_unlock (&m_mutex);
       
        confFile.close();
    }
   
    snprintf (buffer, 500*sizeof (char), "%i", m_rules.size());
   
    STRIGI_LOG_INFO ("strigi.filtermanager", "added " + string(buffer) + " filtering rules")
}

void FilterManager::saveFilter()
{
    if (m_patternFile.empty() || m_pathFile.empty())
        return;
    
    fstream pathFile;
    fstream patternFile;
    pathFile.open(m_pathFile.c_str(), ios::out | ios::trunc);
    patternFile.open(m_patternFile.c_str(), ios::out | ios::trunc);
   
    pthread_mutex_lock (&m_mutex);
   
    // TODO: fix when we'll use a single conf file
    if (pathFile.is_open() && patternFile.is_open())
    {
        for (vector<Filter*>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
        {
            Filter* filter = *iter;

            switch (filter->rtti())
            {
                case PathFilter::RTTI:
                    pathFile << filter->rule() << endl;
                    break;
                case PatternFilter::RTTI:
                    patternFile << filter->rule() << endl;
                    break;
                default:
                    STRIGI_LOG_ERROR ("strigi.filtermanager.saveFilter", "unknown rule RTTI")
                    break;
            }
        }
       
        patternFile.close();
        pathFile.close();
       
        STRIGI_LOG_DEBUG ("strigi.filtermanager", "successfully saved filtering rules")
    }
    else
        STRIGI_LOG_ERROR ("strigi.filtermanager.saveFilter", "unable to save filtering rules");
   
    pthread_mutex_unlock (&m_mutex);
}

bool FilterManager::findMatch(const char* text)
{
    string t (text);
    return findMatch (t);
}

bool FilterManager::findMatch (string& text)
{
    pthread_mutex_lock (&m_mutex);
    
    // check if text is related with strigi configuration directory
    int ret = fnmatch (m_strigidir.c_str(), text.c_str(), 0);
       
    if ((ret != FNM_NOMATCH) && (ret != 0))
        STRIGI_LOG_WARNING ("strigi.filtermanager.PathFilter", "error while applying pattern " + m_strigidir + "over text " + text)
    else if ( ret == 0)
    {
        STRIGI_LOG_DEBUG ("strigi.filtermanager.PathFilter", "Ignoring strigi configuration directory: " + m_strigidir)
        return true;
    }
    
    for (vector<Filter*>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
    {
        Filter* filter = *iter;
        if (filter->match (text))
        {
            pthread_mutex_unlock (&m_mutex);
            return true;
        }
    }
   
    pthread_mutex_unlock (&m_mutex);

    //STRIGI_LOG_DEBUG ("strigi.filtermanager", text + " didn't match any pattern")
    return false;
}

multimap<int,string> FilterManager::getFilteringRules()
{
    multimap<int,string> rules;
    
    for (vector<Filter*>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
    {
        Filter* filter = *iter;
        rules.insert(make_pair (int(filter->rtti()),filter->rule()));
    }

    return rules;
}

void FilterManager::setFilteringRules(const multimap<int, string>& rules)
{
    clearRules();

    map<int,string>::const_iterator iter;
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
}
