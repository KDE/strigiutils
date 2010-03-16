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
#ifndef DBUSOBJECTINTERFACE_H
#define DBUSOBJECTINTERFACE_H
#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>

#include <string>
class DBusObjectInterface {
private:
    const std::string interfacename;
public:
    DBusObjectInterface(const std::string& i) :interfacename(i) {}
    virtual ~DBusObjectInterface() {};
    virtual DBusHandlerResult handleCall(DBusConnection* connection,
        DBusMessage* msg) = 0;
    virtual std::string getIntrospectionXML() = 0;
    const std::string& getInterfaceName() const {
        return interfacename;
    }
};

#endif
