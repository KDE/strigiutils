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

#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>

#include <set>
#include <map>
#include <string>
#include <vector>
#include <strigi/strigiconfig.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

class DBusMessageReader {
private:
    DBusMessage* msg;
    bool ok;

public:
    DBusMessageIter it;
    explicit DBusMessageReader(DBusMessage* msg);
    explicit DBusMessageReader(const DBusMessageReader& r)
            :msg(r.msg), ok(r.ok) {
        dbus_message_ref(msg);
    }
    ~DBusMessageReader() {
        close();
    }
    void close() {
        if (msg) {
            dbus_message_unref(msg);
            msg = 0;
        }
        ok = false;
    }
    DBusMessageReader& operator>>(std::set<std::string>& s);
    DBusMessageReader& operator>>(std::vector<std::string>& s);
    DBusMessageReader& operator>>(std::vector<char>& s);
    DBusMessageReader& operator>>(std::vector<int32_t>& s);
    DBusMessageReader& operator>>(std::vector<uint32_t>& s);
    DBusMessageReader& operator>>(std::string& s);
    DBusMessageReader& operator>>(dbus_int32_t& s);
    DBusMessageReader& operator>>(dbus_uint32_t& s);
    DBusMessageReader& operator>>(dbus_uint64_t& s);
    DBusMessageReader& operator>>(dbus_int64_t& s);
    DBusMessageReader& operator>>(std::multimap<int, std::string>&);
    DBusMessageReader& operator>>(std::vector<std::pair<bool, std::string> >&);
    bool isOk() const { return ok; }
    bool atEnd();
};

#endif
