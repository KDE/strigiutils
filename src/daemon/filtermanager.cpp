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
#include "strigilogging.h"

#include <fnmatch.h>
#include <fstream>

using namespace std;

FilterManager::FilterManager()
{
    pthread_mutex_init(&m_mutex, NULL);
}

FilterManager::~ FilterManager()
{
    saveFilter();
    pthread_mutex_destroy (&m_mutex);
}

void FilterManager::setConfFile (string& confFile)
{
    m_strConfFile = confFile;
    
    loadFilter();
}

void FilterManager::loadFilter()
{
    fstream confFile;
    string rule;
    char buffer [500];
    
    confFile.open(m_strConfFile.c_str(), std::ios::in);
    
    // clear old rules
    m_rules.clear();
    
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
                m_rules.insert (string(buffer));
                STRIGI_LOG_DEBUG ("strigi.filtermanager", "added rule: |" + string(buffer) +"|")
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
    std::fstream confFile; 
    confFile.open(m_strConfFile.c_str(), std::ios::out | std::ios::trunc);
    
    pthread_mutex_lock (&m_mutex);
    
    if (confFile.is_open())
    {
        for (set<string>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
            confFile << *iter << endl; 
        
        confFile.close();
        
        STRIGI_LOG_DEBUG ("strigi.filtermanager", "successfully saved filtering rules to " + m_strConfFile)
    }
    else
        STRIGI_LOG_ERROR ("strigi.filtermanager", "unable to save filtering rules to file " + m_strConfFile);
    
    pthread_mutex_unlock (&m_mutex);
}

bool FilterManager::findMatch(const char* text)
{
    string t (text);
    return findMatch (t);
}

bool FilterManager::findMatch (string& text)
{
    int ret;
    
    pthread_mutex_lock (&m_mutex);
    
    for (set<string>::iterator iter = m_rules.begin(); iter != m_rules.end(); iter++)
    {
        ret = fnmatch (iter->c_str(), text.c_str(), 0);
        
        if ((ret != FNM_NOMATCH) && (ret != 0))
            STRIGI_LOG_WARNING ("strigi.filtermanager", "error while applying pattern " + *iter + "over text " + text)
        else if ( ret == 0)
        {
            pthread_mutex_unlock (&m_mutex);
            STRIGI_LOG_DEBUG ("strigi.filtermanager", text + " matched pattern " + *iter)
            return true;
        }
    }
    
    pthread_mutex_unlock (&m_mutex);
    STRIGI_LOG_DEBUG ("strigi.filtermanager", text + " didn't match any pattern")
    return false;
}

set<string> FilterManager::getFilteringRules()
{
    return m_rules;
}

void FilterManager::setFilteringRules(set<string>& rules)
{
    m_rules.clear();
    for (set<string>::iterator iter = rules.begin(); iter != rules.end(); iter++)
        m_rules.insert (*iter);
}
