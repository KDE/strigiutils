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
#include "dbusmessagewriter.h"
#include <dbus/dbus.h>
using namespace std;

DBusMessageWriter::DBusMessageWriter(DBusConnection* c, DBusMessage* msg)
        :conn(c), reply(dbus_message_new_method_return(msg)), reply_to(msg), error(0) {
    dbus_message_iter_init_append(reply, &it);
}
DBusMessageWriter::DBusMessageWriter(DBusConnection* c, const char* object,
        const char* interface, const char* function)
        :conn(c), reply(dbus_message_new_signal(object, interface, function)),
         reply_to(0), error(0) {
    dbus_message_iter_init_append(reply, &it);
}
DBusMessageWriter::~DBusMessageWriter() {
    if (reply) {
        DBusMessage* msg = (error) ?error :reply;
        dbus_uint32_t serial = 0;
        dbus_connection_send(conn, msg, &serial);
        dbus_connection_flush(conn);
        dbus_message_unref(reply);
        if (error) {
            dbus_message_unref(error);
        }
    }
}
void
DBusMessageWriter::setError(const std::string &e) {
    // allow for only one error
    if (error == 0) {
        // if this is an error in a reply, use the reply in the constructor
        if (reply_to) {
            error = dbus_message_new_error(reply_to,
                "org.freedesktop.DBus.Error.Failed", e.c_str());
        } else {
            error = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
            dbus_message_set_error_name(error,
                "org.freedesktop.DBus.Error.Failed");
        }
    }
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, bool b) {
    dbus_bool_t db = b;
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_BOOLEAN, &db);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, int32_t i) {
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_INT32, &i);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, uint32_t i) {
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_UINT32, &i);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, int64_t i) {
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_INT64, &i);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, double d) {
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_DOUBLE, &d);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::string& s) {
    const char* c = s.c_str();
    dbus_message_iter_append_basic(&w.it, DBUS_TYPE_STRING, &c);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const set<string>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY,
        DBUS_TYPE_STRING_AS_STRING, &sub);
    set<string>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        const char* c = i->c_str();
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &c);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::map<std::string, std::string>& m) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "{ss}", &sub);
    map<string, string>::const_iterator i;
    for (i = m.begin(); i != m.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY, 0, &ssub);
        const char* c = i->first.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->second.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<std::string>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY,
        DBUS_TYPE_STRING_AS_STRING, &sub);
    vector<string>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        const char* c = i->c_str();
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &c);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<int32_t>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "i", &sub);
    vector<int32_t>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        int32_t v = *i;
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &v);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<uint32_t>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "u", &sub);
    vector<uint32_t>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        uint32_t v = *i;
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_UINT32, &v);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
/** map multimap<int,string> to 'a(is)' **/
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::multimap<int, std::string>& m) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "(is)", &sub);
    multimap<int,string>::const_iterator i;
    for (i = m.begin(); i != m.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        int32_t n = i->first;
        const char* s = i->second.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT32, &n);
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &s);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<std::pair<bool, std::string> >& s) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "(bs)", &sub);
    vector<pair<bool,string> >::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        dbus_bool_t n = i->first;
        const char* s = i->second.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_BOOLEAN, &n);
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &s);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<std::pair<std::string, dbus_uint32_t> >& s) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "(su)", &sub);
    vector<pair<string,dbus_uint32_t> >::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        const char* s = i->first.c_str();
        dbus_uint32_t n = i->second;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &s);
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_UINT32, &n);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&w.it, &sub);

    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const std::vector<std::vector<std::string> >& v) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY, "as", &sub);
    vector<vector<string> >::const_iterator i;
    for (i = v.begin(); i != v.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_ARRAY,
            DBUS_TYPE_STRING_AS_STRING, &ssub);
        vector<string>::const_iterator j;
        vector<string>::const_iterator end = (*i).end();
        for (j = (*i).begin(); j != end; ++j) {
            const char* c = j->c_str();
            dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        }
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}
