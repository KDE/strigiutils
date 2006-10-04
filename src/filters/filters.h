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
 
#include <string>

#ifndef FILTERS_H
#define FILTERS_H

/**
 * @class Filter
 * @brief Abstract class for filters
 **/
class Filter
{
    public:
        Filter(const std::string& rule) :m_rule(rule) {}
        virtual ~Filter() {}
        
        /**
        * check if param text matches with rule m_rule
        * @return true if text matches the rule, false otherwise
        **/
        virtual bool match(const std::string& text) = 0;
        const std::string& rule() const { return m_rule; }
        
        /**
        * method used to know the type of a filter instance
        **/
        virtual int rtti() const = 0;
        
    protected:
        const std::string m_rule;
};


/**
* @class PatternFilter
* Class used for generic pattern filters. Be careful: it doesn't use regexp.
*
* <b><i>Example:</i></b> pattern <i>*log</i> will prevent <i>foo.log</i> and <i>prolog.pdf</i> from being indexed, but also directory <i>/home/foo/log/</i> and all its contents
**/
class PatternFilter : public Filter
{
    public:
        PatternFilter(const std::string& rule) : Filter (rule) {}
        ~PatternFilter() {}
        
        enum {RTTI = 1};
        
        bool match(const std::string& text);
        int rtti() const { return RTTI; }
};

/**
 * @class PathFilter
 * Filters all path that are equals to m_rule, and all their subdirs
 * 
 * <b><i>Example:</i></b> path filter <i>/home/foo/bar/</i> will prevent directory <i>/home/foo/bar/</i> and all its contents from being indexed
 **/
class PathFilter : public Filter
{
    public:
        PathFilter(const std::string& rule) : Filter (rule) {}
        ~PathFilter() {}
        
        enum {RTTI = 2};
        
        bool match (const std::string& text);
        int rtti() const { return RTTI; }
};

#endif
