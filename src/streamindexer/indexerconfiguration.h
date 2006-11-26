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

/**
 * This class allows the Indexable to determine how each field should be
 * indexed.
 **/
namespace jstreams {
class IndexerConfiguration {
public:
enum FieldType {
    None      = 0x0001, Binary = 0x0002, Compressed = 0x0004,
    Indexed   = 0x0010, Lazy   = 0x0020, Stored     = 0x0040,
    Tokenized = 0x0100
};

public:
    IndexerConfiguration();
    virtual ~IndexerConfiguration() {}
    virtual bool indexMore() const {return true;}
    virtual bool addMoreText() const {return true;}
    virtual FieldType getIndexType(const std::string& fieldname) const;
};

/**
 * Overloaded operator| that retains the type of the flag when |'ing two
 * field values.
 **/
//IndexerConfiguration::FieldType
//operator|(IndexerConfiguration::FieldType a, IndexerConfiguration::FieldType b);

}
#endif
