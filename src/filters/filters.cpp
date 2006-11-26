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
#include "filters.h"
#include "../daemon/strigilogging.h"

#include "strigi_fnmatch.h"

using namespace std;

bool PatternFilter::match(const string& text) {
	int flags = 0;
#ifdef _WIN32
	flags |= FNM_CASE_BLIND;
#endif
    int ret = fnmatch (m_rule.c_str(), text.c_str(), flags);
       
    if ((ret != FNM_NOMATCH) && (ret != 0))
        STRIGI_LOG_WARNING ("strigi.filtermanager.PatternFilter", "error while applying pattern " + m_rule + "over text " + text)
    else if ( ret == 0)
    {
        STRIGI_LOG_DEBUG ("strigi.filtermanager.PatternFilter", text + " matched pattern " + m_rule)
        return true;
    }
    
    return false;
}

bool PathFilter::match (const string& text) {
	int flags = 0;
#ifdef _WIN32
	flags |= FNM_CASE_BLIND;
#endif

    // create the real pattern, whe have to add a * for globbing
    string realPattern = m_rule;
    
    // remove the trailing / (unix) or \ (win) 
    if ((realPattern[realPattern.size() - 1] == '/') || (realPattern[realPattern.size()-1] == '\\'))
        realPattern.resize(realPattern.size() - 1);
    
    realPattern+='*';
    
    int ret = fnmatch (realPattern.c_str(), text.c_str(), flags);
       
    if ((ret != FNM_NOMATCH) && (ret != 0))
        STRIGI_LOG_WARNING ("strigi.filtermanager.PathFilter", "error while applying pattern " + m_rule + "over text " + text)
    else if ( ret == 0)
    {
        STRIGI_LOG_DEBUG ("strigi.filtermanager.PathFilter", text + " matched pattern " + m_rule)
        return true;
    }
    
    return false;
}

