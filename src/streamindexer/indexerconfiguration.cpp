/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "indexerconfiguration.h"
#include "jstreamsconfig.h"
#include <fnmatch.h>
using namespace std;
using namespace jstreams;

IndexerConfiguration::FieldType
operator|(IndexerConfiguration::FieldType a, IndexerConfiguration::FieldType b){
    return static_cast<IndexerConfiguration::FieldType>((int)a|(int)b);
}
IndexerConfiguration::IndexerConfiguration() {
    Pattern svn;
    svn.pattern = ".svn";
    svn.include = false;
    svn.matchfullpath = false;
    //svn.matchdir = true;
    patterns.push_back(svn);
}
/**
 * Placeholder implementation that agrees to everything and only makes a
 * difference for text, because it should be tokenized.
 **/
IndexerConfiguration::FieldType
IndexerConfiguration::getIndexType(const std::string& fieldname) const {
    return (fieldname == "text") ? Tokenized|Stored|Indexed
                                : Stored|Indexed;
}
bool
IndexerConfiguration::indexPathFragment(const string& pathfragment) const {
    return true;
}
bool
IndexerConfiguration::indexFile(const char* filename) const {
    vector<Pattern>::const_iterator i;
    for (i = patterns.begin(); i != patterns.end(); ++i) {
    }
    return true;
}
bool
IndexerConfiguration::indexFile(const char* path, const char* filename) const {
    vector<Pattern>::const_iterator i;
    for (i = patterns.begin(); i != patterns.end(); ++i) {
        bool match;
        if (i->matchfullpath) {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), path, 0);
        } else {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), filename, 0);
        }
        if (match) {
            return i->include;
        }
    }
    return true;
}
bool
IndexerConfiguration::indexDir(const char* filename) const {
    return true;
}
bool
IndexerConfiguration::indexDir(const char* path, const char* filename) const {
/*    vector<Pattern>::const_iterator i;
    for (i = dirpatterns.begin(); i != dirpatterns.end(); ++i) {
        bool match;
        if (i->matchfullpath) {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), path, 0);
        } else {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), filename, 0);
        }
        if (match) {
            printf("dir '%s' %i\n", path, i->include);
            return i->include;
        }
    }*/
    return true;
}
void
IndexerConfiguration::printFilters() const {
    vector<Pattern>::const_iterator i;
    for (i = patterns.begin(); i != patterns.end(); ++i) {
    }
}
void
IndexerConfiguration::setFilters(
        const std::vector<std::pair<bool,std::string> >& filters) {
    vector<pair<bool,string> >::const_iterator i;
    patterns.clear();
    for (i = filters.begin(); i != filters.end(); ++i) {
        string s = i->second;
        if (s.length()) {
            Pattern p;
            p.include = i->first;
            if (s[s.length()-1] == '/') { // directory pattern
                s += '*';
                p.matchfullpath = true;
            } else {
                p.matchfullpath = s.find('/') != string::npos;
            }
            p.pattern = s;
            patterns.push_back(p);
        }
    }
}
