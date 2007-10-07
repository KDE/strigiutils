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
#include "dbusobjectcallhandler.h"
#include "dbusobjectinterface.h"
#include "dbusmessagewriter.h"
#include <sstream>
using namespace std;

class DBusInterface : public DBusObjectInterface {
private:
    DBusObjectCallHandler* object;
public:
    DBusInterface(DBusObjectCallHandler* o)
        : DBusObjectInterface("org.freedesktop.DBus"),
          object(o) {}
    ~DBusInterface() {}
};


class IntrospectionInterface : public DBusObjectInterface {
private:
    DBusObjectCallHandler* object;
    DBusHandlerResult handleCall(DBusConnection* connection, DBusMessage* msg);
    void introspect(DBusMessage* msg, DBusConnection* conn);
public:
    IntrospectionInterface(DBusObjectCallHandler* o)
        : DBusObjectInterface("org.freedesktop.DBus.Introspectable"),
          object(o) {}
    ~IntrospectionInterface() {}
    std::string getIntrospectionXML();
};
std::string
IntrospectionInterface::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "  <interface name='"+getInterfaceName()+"'>\n"
    << "    <method name='Introspect'>\n"
    << "      <arg name='xml_data' type='s' direction='out'/>\n"
    << "    </method>\n  </interface>\n";
    return xml.str();
}
DBusHandlerResult
IntrospectionInterface::handleCall(DBusConnection* conn, DBusMessage* msg) {
    const char* i = getInterfaceName().c_str();
    if (dbus_message_is_method_call(msg, i, "Introspect")) {
        introspect(msg, conn);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
void
IntrospectionInterface::introspect(DBusMessage* msg, DBusConnection* con) {
    DBusMessageWriter writer(con, msg);
    writer << object->getIntrospectionXML();
}

const DBusObjectPathVTable DBusObjectCallHandler::vtable = {
    NULL,
    message_function,
    NULL,
    NULL,
    NULL,
    NULL
};
DBusObjectCallHandler::DBusObjectCallHandler(const std::string& path)
        :objectpath(path) {
    introspection = new IntrospectionInterface(this);
    addInterface(introspection);
}
DBusObjectCallHandler::~DBusObjectCallHandler() {
    delete introspection;
}
void
DBusObjectCallHandler::addInterface(DBusObjectInterface* interface) {
    interfaces[interface->getInterfaceName()] = interface;
}
DBusHandlerResult
DBusObjectCallHandler::message_function(DBusConnection* connection,
        DBusMessage* msg, void* object) {
    DBusObjectCallHandler* handler
        = static_cast<DBusObjectCallHandler*>(object);
    return handler->handleCall(msg);
}
DBusHandlerResult
DBusObjectCallHandler::handleCall(DBusMessage* msg){
    const char* iface = dbus_message_get_interface(msg);
    map<string, DBusObjectInterface*>::const_iterator i;
    if (iface == 0) { // no interface was specified, try all interfaces
        DBusHandlerResult r;
        for (i=interfaces.begin(); i!=interfaces.end(); ++i) {
            r = i->second->handleCall(conn, msg);
            if (r == DBUS_HANDLER_RESULT_HANDLED) {
                return r;
            }
        }
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    } else {
        i = interfaces.find(dbus_message_get_interface(msg));
        if (i == interfaces.end()) {
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
        return i->second->handleCall(conn, msg);
    }
}
void
DBusObjectCallHandler::registerOnConnection(DBusConnection* c) {
    conn = c;
    dbus_connection_register_object_path(c, objectpath.c_str(),
        &DBusObjectCallHandler::vtable, this);
}
std::string
DBusObjectCallHandler::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "<node name='" << objectpath << "'>\n";
    map<string, DBusObjectInterface*>::const_iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i) {
        xml << i->second->getIntrospectionXML();
    }
    xml << "</node>\n";
    return xml.str();
}
