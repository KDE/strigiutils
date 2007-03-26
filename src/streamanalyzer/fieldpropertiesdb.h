/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef FIELDPROPERIESDB_H
#define FIELDPROPERIESDB_H

#include "jstreamsconfig.h"
#include "fieldproperties.h"
#include <map>
#include <vector>

namespace Strigi {

class FieldProperties;
class FieldProperties;
/**
 * This class is the default implementation for getting at metadata related to
 * properties. It loads the information from files with a similar syntax to
 * the freedesktop desktop files.
 * There is one instance of the class per running application.
 * The implemntation of this class might use a performance enhanced binary cache
 * of the property files.
 **/
class STREAMANALYZER_EXPORT FieldPropertiesDb {
private:
    class Private;
    Private* const p;
public:
    static FieldPropertiesDb& db();
    FieldPropertiesDb();
    ~FieldPropertiesDb();
    const FieldProperties& properties(const std::string& uri) const;
    const std::map<std::string, FieldProperties>& allProperties() const;
};

class FieldProperties::Private {
friend class FieldPropertiesDb;
public:
    static const std::string empty;
    std::string uri;
    std::string name;
    std::string typeuri;
    std::string description;
    std::map<std::string,FieldProperties::Localized> localized;
    std::vector<std::string> parentUris;

    Private() {}
    Private(const Private&p) { *this = p; }
    Private(const std::string& i) :uri(i) {}
    void clear();
};

}

#endif
