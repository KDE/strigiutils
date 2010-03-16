/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#ifndef STRIGI_FIELDPROPERTIES_PRIVATE_H
#define STRIGI_FIELDPROPERTIES_PRIVATE_H

#include <strigi/fieldproperties.h>
#include <strigi/fieldtypes.h>

namespace {
    const std::string& empty() {
        static std::string e;
        return e;
    }
}

namespace Strigi {

class FieldProperties::Private {
friend class FieldPropertiesDb;
public:
    std::string uri;
    std::string name;
    std::string alias;
    std::string typeuri;
    std::string description;
    std::map<std::string,FieldProperties::Localized> localized;
    std::vector<std::string> locales;
    std::vector<std::string> parentUris;
    std::vector<std::string> childUris;
    std::vector<std::string> applicableClasses;
    /**< The field should be stored as binary data. (0x0001) 		*/
    bool binary;
    /**< If the field is stored, the data should be compressed. (0x0002)*/
    bool compressed;
    /**< The field should be indexed. (0x0004)                          */
    bool indexed;
    /**< The field should be stored. (0x0020)                           */
    bool stored;
    /**< If the field contains text, it should be tokenized. (0x0040)   */
    bool tokenized;
    /** Should the field contain only unique values **/
    bool uniquevalues;
    /** The minimal number of occurrences required for this field. **/
    int min_cardinality;
    /** The maximal number of occurrences allowed for this field. **/
    int max_cardinality;

    Private() :binary(false), compressed(false), indexed(true), stored(false),
            tokenized(true), uniquevalues(true), min_cardinality(0),
            max_cardinality(-1) {
        typeuri = FieldRegister::stringType;
    }
    Private(const Private&p) { *this = p; }
    Private(const std::string& i) :uri(i) { }
    void clear();
};

class ClassProperties::Private {
friend class FieldPropertiesDb;
public:
    std::string uri;
    std::string name;
    std::string description;
    std::map<std::string,ClassProperties::Localized> localized;
    std::vector<std::string> locales;
    std::vector<std::string> parentUris;
    std::vector<std::string> childUris;
    std::vector<std::string> applicableProperties;

    Private() {}
    Private(const Private&p) { *this = p; }
    Private(const std::string& i) :uri(i) { }
    void clear();
};

}

#endif
