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

#include "filters.h"
#include "filtermanager.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main()
{
    multimap <int, string> rules;
    
    // add filtering rules
    rules.insert (make_pair(int(PathFilter::RTTI), string("/home/user/foo/")));
    rules.insert (make_pair(int(PathFilter::RTTI), string("/home/user/bar/")));
    rules.insert (make_pair(int(PathFilter::RTTI), string("*test*")));
    
    FilterManager filtermanager;
    filtermanager.setFilteringRules(rules);
    
    vector<string> text;
    text.push_back("/home/user/foo/");
    text.push_back(string("/foo/"));
    text.push_back(string("/home/user/bar/foo/test"));
    
    for (vector<string>::iterator iter = text.begin(); iter != text.end(); iter++)
    {
        if (!filtermanager.findMatch( *iter))
            cout << *iter << " doesn't match any filtering rule!\n";
        else
            cout << *iter << " match a filtering rule!\n";
    }
    
    return 0;
}

