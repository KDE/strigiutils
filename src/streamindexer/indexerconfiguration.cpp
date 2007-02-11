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
#include "strigi_fnmatch.h"
using namespace std;
using namespace jstreams;

IndexerConfiguration::FieldType
operator|(IndexerConfiguration::FieldType a, IndexerConfiguration::FieldType b){
    return static_cast<IndexerConfiguration::FieldType>((int)a|(int)b);
}
IndexerConfiguration::IndexerConfiguration() {
}
/**
 * Placeholder implementation that agrees to everything and only makes a
 * difference for text, because it should be tokenized.
 **/
IndexerConfiguration::FieldType
IndexerConfiguration::getIndexType(const RegisteredField* field)
        const {
    return Tokenized|Stored|Indexed;
}
bool
IndexerConfiguration::indexFile(const char* path, const char* filename) const {
    vector<Pattern>::const_iterator i;
    for (i = patterns.begin(); i != patterns.end(); ++i) {
        bool match;
        if (i->matchfullpath) {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), path,
                FNM_PERIOD);
        } else {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), filename,
                FNM_PERIOD);
        }
        if (match) {
            return i->include;
        }
    }
    return true;
}
bool
IndexerConfiguration::indexDir(const char* path, const char* filename) const {
    vector<Pattern>::const_iterator i;
    for (i = dirpatterns.begin(); i != dirpatterns.end(); ++i) {
        bool match;
        if (i->matchfullpath) {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), path,
                FNM_PERIOD);
        } else {
            match = FNM_NOMATCH != fnmatch(i->pattern.c_str(), filename,
                FNM_PERIOD);
        }
        if (match) {
            return i->include;
        }
    }
    return true;
}
void
IndexerConfiguration::printFilters() const {
    vector<Pattern>::const_iterator i;
    for (i = patterns.begin(); i != patterns.end(); ++i) {
    }
}
/**
 * We need to transform the incoming patterns like this: */
 //  a   -> a
 //  .*  -> .*
 //  *   -> *
 //  a/  -> */a/*
 //  .*/ -> */.*
void
IndexerConfiguration::setFilters(
        const std::vector<std::pair<bool,std::string> >& f) {
    filters = f;
    vector<pair<bool,string> >::const_iterator i;
    patterns.clear();
    dirpatterns.clear();
    bool hadinclude = false;
    for (i = filters.begin(); i != filters.end(); ++i) {
        string s = i->second;
        if (s.length()) {
            hadinclude |= i->first;
            Pattern p;
            p.include = i->first;
            size_t sp = s.rfind('/');
            if (sp == string::npos) {
                p.matchfullpath = false;
            } else {
                if (sp == s.length()-1) { // directory pattern
                    sp = s.rfind('/', s.length()-2);
                    if (!hadinclude) { // can exclude entire directory
                        p.matchfullpath = sp != string::npos;
                        p.pattern = s.substr(0, s.length()-1);
                        dirpatterns.push_back(p);
                        continue;
                    }
                    if (s.length() == 1 || s[s.length()-2] != '*') {
                        s += '*';
                    }
                    if (sp == string::npos
                            && s[0] != '*') {
                        s = "*/" + s;
                    }
                }
                p.matchfullpath = true;
            }
            p.pattern = s;
            patterns.push_back(p);
        }
    }
}
