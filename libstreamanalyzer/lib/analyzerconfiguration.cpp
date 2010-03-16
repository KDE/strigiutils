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
#include <strigi/analyzerconfiguration.h>
#include <strigi/strigiconfig.h>
#include "strigi_fnmatch.h"
#include <strigi/fieldproperties.h>
#include <strigi/fieldpropertiesdb.h>
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

    bool indexArchiveContents;

    AnalyzerConfigurationPrivate()
        : indexArchiveContents( true ) {
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
 * Returns indexing flags from the fieldproperties DB associated to
 * the registered field.
 **/
AnalyzerConfiguration::FieldType
AnalyzerConfiguration::indexType(const RegisteredField* field) const {
    AnalyzerConfiguration::FieldType type = None;
    const FieldProperties& prop = field->properties();
    if (prop.binary())		{ type = type|Binary;		}
    if (prop.compressed())	{ type = type|Compressed;	}
    if (prop.indexed())		{ type = type|Indexed;		}
    if (prop.stored())		{ type = type|Stored;		}
    if (prop.tokenized())	{ type = type|Tokenized;	}
    return type;
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
AnalyzerConfiguration::indexArchiveContents() const {
    return p->indexArchiveContents;
}
void
AnalyzerConfiguration::setIndexArchiveContents( bool b ) {
    p->indexArchiveContents = b;
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
    for (i = p->m_filters.begin(); i != p->m_filters.end(); ++i) {
        string s = i->second;
        if (s.length()) {
            AnalyzerConfigurationPrivate::Pattern p;
            p.include = i->first;
            size_t sp = s.rfind('/');
            if (sp == s.length()-1) { // directory pattern (it ends with '/')
                // if the path contains another '/', match the entire path
                sp = s.rfind('/', s.length()-2);
                p.matchfullpath =  sp != string::npos;
                // remove the terminating '/'
                p.pattern = s.substr(0, s.length()-1);
                this->p->m_dirpatterns.push_back(p);
            } else { // file pattern
                // if the path contains a '/', match the entire path
                p.matchfullpath =  sp != string::npos;
                p.pattern = s;
                this->p->m_patterns.push_back(p);
            }
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
