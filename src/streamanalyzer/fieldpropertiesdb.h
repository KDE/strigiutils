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
#ifndef STRIGI_FIELDPROPERTIESDB_H
#define STRIGI_FIELDPROPERTIESDB_H

#include <strigi/strigiconfig.h>
#include "fieldproperties.h"
#include "classproperties.h"
#include <map>
#include <vector>

namespace Strigi {

class FieldProperties;
/**
 * This class is the default implementation for getting at metadata related to
 * properties. It loads the information from RDF/XML files.
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
    const FieldProperties& propertiesByAlias(const std::string& alias) const;
    const std::map<std::string, FieldProperties>& allProperties() const;

    const ClassProperties& classes(const std::string& uri) const;
    const std::map<std::string, ClassProperties>& allClasses() const;

    /**
     * This is a simple method for adding fields programatically instead of
     * via read files. This is useful for introspecting fields for which no
     * addfield has yet been written.
     * At a later stage we might remove this function and demand that all fields
     * be registered over fieldproperties files. Right now, this helps in
     * building the ontologies without demanding too much of the analyzer
     * writers.
     **/
    void addField(const std::string& key, const std::string& type,
        const std::string& parent);
    /**
     * This method reflects the change in API when all field properties are
     * loaded from the .fieldproperties database and not specified in runtime
     * by calling plugins.
     * This call is a stub for those who want to add values for the field,
     * which is not added to the central ontology database.
     * 
     * NOTE: the default type is stringType
     *       and there is no parent by default
     *       the only property which is set is key or fieldname
     **/
    void addField(const std::string& key);
};


}

#endif
