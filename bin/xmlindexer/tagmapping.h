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
#ifndef TAGMAPPING_H
#define TAGMAPPPING_H

#include <map>
#include <string>

class TagMapping {
private:
    std::map<std::string, std::string> m_namespaces;
    std::map<std::string, std::string> mapping;
public:
    TagMapping(const char* mappingfile);
    const std::map<std::string, std::string>& namespaces() const {
        return m_namespaces;
    }
    const std::string& map(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator i
            = mapping.find(key);
        return (i == mapping.end()) ?key :i->second;
    }
};

#endif
