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
#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <vector>
#include <map>
#include <string>
#include <dbus/dbus.h>

class DBusObjectInterface;
class DBusHandler {
private:
    int quitpipe[2];
    DBusConnection* conn;
    std::map<std::string, std::vector<DBusObjectInterface*> > interfaces;
    std::vector<std::string> busnames;
public:
    DBusHandler();
    void addBusName(const std::string& busname) {
        busnames.push_back(busname);
    }
    void addInterface(const std::string& objectname, DBusObjectInterface* i) {
        interfaces[objectname].push_back(i);
    }
    DBusConnection* connection() const { return conn; }
    bool handle();
    void stop();
};

#endif
