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
#ifndef DBUSOBJECTCALLHANDLER_H
#define DBUSOBJECTCALLHANDLER_H
#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>

#include <string>
#include <map>
class DBusObjectInterface;

class DBusObjectCallHandler {
private:
    static DBusHandlerResult message_function(DBusConnection* connection,
        DBusMessage* message, void* user_data);
    DBusConnection* conn;
    DBusObjectInterface* introspection;

    const std::string objectpath;
    std::map<std::string, DBusObjectInterface*> interfaces;
public:
    static const DBusObjectPathVTable vtable;
    DBusObjectCallHandler(const std::string& path);
    ~DBusObjectCallHandler();
    void addInterface(DBusObjectInterface* interface);
    void registerOnConnection(DBusConnection* c);
    std::string getIntrospectionXML();
    const char* getObjectPath() const { return objectpath.c_str(); }
    DBusHandlerResult handleCall(DBusMessage* msg);
};

#endif
