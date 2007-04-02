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
#ifndef FIELDPROPERTIES_H
#define FIELDPROPERTIES_H
#include "strigiconfig.h"
#include <string>
#include <vector>
#include <map>

namespace Strigi {

class STREAMANALYZER_EXPORT FieldProperties {
public:
    class Private;
private:
    Private* const p;
public:
    struct Localized {
        std::string name;
        std::string description;
    };
    FieldProperties();
    FieldProperties(const FieldProperties&);
    FieldProperties(const Private&);
    FieldProperties(const std::string& key);
    ~FieldProperties();
    const FieldProperties& operator=(const FieldProperties&);
    bool valid() const;
    const std::string& uri() const;
    const std::string& name() const;
    const std::string& typeUri() const;
    const std::string& description() const;
    const std::map<std::string, Localized>& localized() const;
    const std::string& localizedName(const std::string& locale) const;
    const std::string& localizedDescription(const std::string& locale) const;
    const std::vector<std::string>& parentUris() const;
};

}
#endif
