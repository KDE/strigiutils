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
#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <string>
#include <set>
#include <map>
#include <vector>
#include <strigi/strigiconfig.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <dbus/dbus.h>

class DBusMessageWriter {
    DBusConnection* const conn;
    DBusMessage* const reply;
    DBusMessage* const reply_to;
    DBusMessage* error;
public:
    DBusMessageIter it;

    DBusMessageWriter(DBusConnection* conn, DBusMessage* msg);
    DBusMessageWriter(DBusConnection* conn, const char* objectpath,
        const char* interface, const char* function);
    DBusMessageWriter() :conn(0), reply(0), reply_to(0), error(0) { }
    ~DBusMessageWriter();
    void setError(const std::string& error);
};

DBusMessageWriter& operator<<(DBusMessageWriter&, bool b);
DBusMessageWriter& operator<<(DBusMessageWriter&, int32_t i);
DBusMessageWriter& operator<<(DBusMessageWriter&, uint32_t i);
DBusMessageWriter& operator<<(DBusMessageWriter&, int64_t i);
DBusMessageWriter& operator<<(DBusMessageWriter&, double d);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::string& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::set<std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<int32_t>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<uint32_t>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::map<std::string, std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::pair<bool, std::string> >& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::multimap<int, std::string>&);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::pair<std::string, dbus_uint32_t> >&);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::vector<std::string> >&);

#endif
