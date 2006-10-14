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
#ifndef DBUSMESSAGEREADER_H
#define DBUSMESSAGEREADER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <set>
#include <map>
#include <string>
#include <vector>

class DBusMessageReader {
private:
    DBusMessage* msg;
    DBusMessageIter it;
    bool ok;

    void close() {
        if (msg) {
            dbus_message_unref(msg);
            msg = 0;
        }
        ok = false;
    }
public:
    explicit DBusMessageReader(DBusMessage* msg);
    ~DBusMessageReader() {
        close();
    }
    DBusMessageReader& operator>>(std::set<std::string>& s);
    DBusMessageReader& operator>>(std::vector<char>& s);
    DBusMessageReader& operator>>(std::string& s);
    DBusMessageReader& operator>>(dbus_uint32_t& s);
    DBusMessageReader& operator>>(int32_t& s);
    DBusMessageReader& operator>>(dbus_uint64_t& s);
    DBusMessageReader& operator>>(int64_t& s);
    DBusMessageReader& operator>>(std::multimap<int, std::string>&);
    DBusMessageReader& operator>>(time_t&);
    bool isOk() const { return ok; }
};

#endif
