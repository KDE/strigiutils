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
#ifndef STRIGI_CLASSPROPERTIES_H
#define STRIGI_CLASSPROPERTIES_H
#include <strigi/strigiconfig.h>
#include <string>
#include <vector>
#include <map>

namespace Strigi {

class STREAMANALYZER_EXPORT ClassProperties {
public:
    class Private;
private:
    Private* const p;
public:
    struct Localized {
        std::string name;
        std::string description;
    };
    ClassProperties();
    ClassProperties(const ClassProperties&);
    ClassProperties(const Private&);
    ClassProperties(const std::string& key);
    ~ClassProperties();
    const ClassProperties& operator=(const ClassProperties&);
    bool valid() const;
    const std::string& uri() const;
    const std::string& name() const;
    const std::string& description() const;
    const std::vector<std::string>& locales() const;
    const std::string& localizedName(const std::string& locale) const;
    const std::string& localizedDescription(const std::string& locale) const;
    const std::vector<std::string>& parentUris() const;
    const std::vector<std::string>& childUris() const;
    const std::vector<std::string>& applicableProperties() const;
};

}
#endif
