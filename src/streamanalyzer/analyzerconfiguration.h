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
#ifndef INDEXERCONFIGURATION_H
#define INDEXERCONFIGURATION_H
#include <string>
#include <vector>

#include "strigiconfig.h"
#include "fieldtypes.h"

namespace Strigi {
class StreamEndAnalyzerFactory;
class StreamThroughAnalyzerFactory;
class StreamSaxAnalyzerFactory;
class StreamLineAnalyzerFactory;
class StreamEventAnalyzerFactory;
class FieldRegister;

/**
 * This class allows the AnalysisResult to determine how each field should be
 * indexed.
 **/
class STREAMANALYZER_EXPORT AnalyzerConfiguration {
public:
enum FieldType {
    None      = 0x0000,
    Binary = 0x0001, Compressed = 0x0002, Indexed   = 0x0004,
    Lazy   = 0x0010, Stored     = 0x0020, Tokenized = 0x0040
};
private:
    struct Pattern {
        std::string pattern;
        bool matchfullpath;
        bool include;
    };
    std::vector<Pattern> m_patterns;
    std::vector<Pattern> m_dirpatterns;
    std::vector<std::pair<bool,std::string> > m_filters;
    FieldRegister m_fieldregister;

public:
    AnalyzerConfiguration();
    virtual ~AnalyzerConfiguration() {}
    /**
     * A path fragment is e.g. a.txt for folder/a.txt
     * @param path should be for example /folder/a.txt
     * @param filename should be for example a.txt
     **/
    virtual bool indexFile(const char* path, const char* filename) const;
    virtual bool indexDir(const char* path, const char* filename) const;
    void printFilters() const;
    virtual bool useFactory(StreamEndAnalyzerFactory*) const { return true; }
    virtual bool useFactory(StreamThroughAnalyzerFactory*) const {return true; }
    virtual bool useFactory(StreamSaxAnalyzerFactory*) const {return true; }
    virtual bool useFactory(StreamEventAnalyzerFactory*) const {return true; }
    virtual bool useFactory(StreamLineAnalyzerFactory*) const {return true; }
    virtual bool indexMore() const {return true;}
    virtual bool addMoreText() const {return true;}
    virtual FieldType indexType(const Strigi::RegisteredField* f) const;

    void setFilters(const std::vector<std::pair<bool,std::string> >& filters);
    const std::vector<std::pair<bool,std::string> >& filters() const {
        return m_filters;
    }
    FieldRegister& fieldRegister() { return m_fieldregister; }
    const FieldRegister& fieldRegister() const { return m_fieldregister; }
};

/**
 * Overloaded operator| that retains the type of the flag when |'ing two
 * field values.
 **/
//AnalyzerConfiguration::FieldType
//operator|(AnalyzerConfiguration::FieldType a, AnalyzerConfiguration::FieldType b);

}
#endif
