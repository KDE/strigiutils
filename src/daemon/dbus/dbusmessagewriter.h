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
#ifndef DBUSMESSAGEWRITER_H
#define DBUSMESSAGEWRITER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>
#include <set>
#include <map>
#include <vector>

#include "../clientinterface.h"

class DBusMessageWriter {
    DBusConnection* conn;
    DBusMessage* reply;
    DBusMessage* error;
    DBusMessageIter it;
public:
    DBusMessageWriter(DBusConnection* conn, DBusMessage* msg);
    ~DBusMessageWriter();
    void setError(const std::string& error);
    DBusMessageWriter& operator<<(bool b);
    DBusMessageWriter& operator<<(int32_t i);
    DBusMessageWriter& operator<<(int64_t i);
    DBusMessageWriter& operator<<(double d);
    DBusMessageWriter& operator<<(const std::string& s);
    DBusMessageWriter& operator<<(const std::set<std::string>& s);
    DBusMessageWriter& operator<<(const std::vector<std::string>& s);
    DBusMessageWriter& operator<<(const std::map<std::string, std::string>& s);
    DBusMessageWriter& operator<<(const ClientInterface::Hits& s);
    DBusMessageWriter& operator<<(const std::multimap<int, std::string>&);
};
#endif
