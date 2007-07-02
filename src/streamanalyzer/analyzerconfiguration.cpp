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
#include "analyzerconfiguration.h"
#include "strigiconfig.h"
#include "strigi_fnmatch.h"
#include "fieldproperties.h"
#include "fieldpropertiesdb.h"
using namespace std;
using namespace Strigi;

class Strigi::AnalyzerConfigurationPrivate {
public:
    /**
     * @brief Describes a pattern to be applied when deciding whether to
     * index a file or directory.
     */
    struct Pattern {
        std::string pattern; /**< The pattern itself. */
        bool matchfullpath; /**< Apply the pattern to the full path, rather
                                 than just the file/directory name. */
        bool include; /**< True: a match to this pattern should cause the file
                           or directory to be indexed.
                           False: a match to this pattern should prevent the
                           file or directory from being indexed. */
    };
    /**
     * @brief Patterns to be applied to file names or paths.
     */
    std::vector<Pattern> m_patterns;
    /**
     * @brief Patterns to be applied to directory names or paths.
     */
    std::vector<Pattern> m_dirpatterns;
    /**
     * @brief The original filters from which @c m_patterns and
     * @c m_dirpatterns were constructed.
     */
    std::vector<std::pair<bool,std::string> > m_filters;
    FieldRegister m_fieldregister;

    AnalyzerConfigurationPrivate() {
    }
};

AnalyzerConfiguration::FieldType
operator|(AnalyzerConfiguration::FieldType a, AnalyzerConfiguration::FieldType b){
    return static_cast<AnalyzerConfiguration::FieldType>((int)a|(int)b);
}
AnalyzerConfiguration::AnalyzerConfiguration()
        :p(new AnalyzerConfigurationPrivate()) {
//    fprintf(stderr, "AnalyzerConfiguration\n");
    FieldPropertiesDb::db();
}
AnalyzerConfiguration::~AnalyzerConfiguration() {
    delete p;
}
/**
 * Placeholder implementation that agrees to everything and only makes a
 * difference for text, because it should be tokenized.
 **/
AnalyzerConfiguration::FieldType
AnalyzerConfiguration::indexType(const RegisteredField* field)
        const {
    return Tokenized|Stored|Indexed;
}
bool
AnalyzerConfiguration::indexFile(const char* path, const char* filename) const {
    vector<AnalyzerConfigurationPrivate::Pattern>::const_iterator i;
    for (i = p->m_patterns.begin(); i != p->m_patterns.end(); ++i) {
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
AnalyzerConfiguration::indexDir(const char* path, const char* filename) const {
    vector<AnalyzerConfigurationPrivate::Pattern>::const_iterator i;
    for (i = p->m_dirpatterns.begin(); i != p->m_dirpatterns.end(); ++i) {
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
AnalyzerConfiguration::printFilters() const {
    vector<AnalyzerConfigurationPrivate::Pattern>::const_iterator i;
    for (i = p->m_patterns.begin(); i != p->m_patterns.end(); ++i) {
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
AnalyzerConfiguration::setFilters(
        const std::vector<std::pair<bool,std::string> >& f) {
    p->m_filters = f;
    vector<pair<bool,string> >::const_iterator i;
    p->m_patterns.clear();
    p->m_dirpatterns.clear();
    bool hadinclude = false;
    for (i = p->m_filters.begin(); i != p->m_filters.end(); ++i) {
        string s = i->second;
        if (s.length()) {
            hadinclude = hadinclude || i->first;
            AnalyzerConfigurationPrivate::Pattern p;
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
                        this->p->m_dirpatterns.push_back(p);
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
            this->p->m_patterns.push_back(p);
        }
    }
}
const std::vector<std::pair<bool,std::string> >&
AnalyzerConfiguration::filters() const {
    return p->m_filters;
}
FieldRegister&
AnalyzerConfiguration::fieldRegister() {
    return p->m_fieldregister;
}
const FieldRegister&
AnalyzerConfiguration::fieldRegister() const {
    return p->m_fieldregister;
}
