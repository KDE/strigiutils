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
#include "jstreamsconfig.h"
#include "filtermanager.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <assert.h>

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
    
	vector<bool> correct;
    vector<string> text;
    text.push_back("/home/user/foo/"); correct.push_back(true);
    text.push_back(string("/foo/")); correct.push_back(false); //shouldn't match
    text.push_back(string("/home/user/bar/foo/test")); correct.push_back(true);
    
	vector<bool>::iterator iter2 = correct.begin();
    for (vector<string>::iterator iter = text.begin(); iter != text.end(); iter++)
    {
		bool shouldMatch = *iter2;
        if (!filtermanager.findMatch( *iter)){
            cout << *iter << " doesn't match any filtering rule!\n";
			assert(! shouldMatch );
		}else{
            cout << *iter << " matches a filtering rule...\n";
			assert( shouldMatch );
		}

		
		iter2++;
    }
    
    return 0;
}

