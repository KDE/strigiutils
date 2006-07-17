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
#ifndef SEARCHSTORAGE
#define SEARCHSTORAGE

#include <string>

class PropertyWriter {
public:
    enum PropertyType {STRING, INT, BINARY};
protected:
    PropertyType type;
    std::string propertyname;
public:
    virtual ~PropertyWriter() {}
    std::string getPropertyName() { return propertyname; }
    PropertyType getPropertyType() const { return type; }
    virtual char writeProperty(long fileid, const char *value, int numBytes) = 0;
    virtual char writeProperty(long fileid, const void *value, int numBytes) = 0;
    virtual char writeProperty(long fileid, int value) = 0;
};

class SearchStorage {
public:
    virtual ~SearchStorage() {}
    virtual void defineProperty(const char *propertyname, PropertyWriter::PropertyType type, bool onetoone) = 0;
    virtual PropertyWriter *getPropertyWriter(const char *propertyname) = 0;
    virtual long addFile(long parent, const char *name) = 0;
};

#endif
