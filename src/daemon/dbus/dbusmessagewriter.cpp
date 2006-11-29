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
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include "textutils.h"
using namespace std;

DBusMessageWriter::DBusMessageWriter(DBusConnection* c, DBusMessage* msg)
        :conn(c), error(0) {
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &it);
}
DBusMessageWriter::~DBusMessageWriter() {
    DBusMessage* msg = (error) ?error :reply;
    dbus_uint32_t serial = 0;
    dbus_connection_send(conn, msg, &serial);
    dbus_connection_flush(conn);
    dbus_message_unref(reply);
    if (error) {
        dbus_message_unref(error);
    }
}
void
DBusMessageWriter::setError(const std::string &e) {
    error = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
    dbus_message_set_error_name(error, e.c_str());
}
DBusMessageWriter&
DBusMessageWriter::operator<<(bool b) {
    dbus_message_iter_append_basic(&it, DBUS_TYPE_BOOLEAN, &b);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(int32_t i) {
    dbus_message_iter_append_basic(&it, DBUS_TYPE_INT32, &i);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(int64_t i) {
    dbus_message_iter_append_basic(&it, DBUS_TYPE_INT64, &i);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(double d) {
    dbus_message_iter_append_basic(&it, DBUS_TYPE_DOUBLE, &d);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::string& s) {
    const char* c = s.c_str();
    dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &c);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const set<string>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY,
        DBUS_TYPE_STRING_AS_STRING, &sub);
    set<string>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        const char* c = i->c_str();
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &c);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::map<std::string, std::string>& m) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY, "{ss}", &sub);
    map<string, string>::const_iterator i;
    for (i = m.begin(); i != m.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY, 0, &ssub);
        const char* c = i->first.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->second.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::vector<std::string>& s) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY,
        DBUS_TYPE_STRING_AS_STRING, &sub);
    vector<string>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        const char* c = i->c_str();
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &c);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
/* yes this is ugly, i have to change the interface */
DBusMessageWriter&
DBusMessageWriter::operator<<(const ClientInterface::Hits& s) {
    DBusMessageIter sub;
    DBusMessageIter ssub;
    DBusMessageIter sssub;
    DBusMessageIter ssssub;
    DBusMessageIter sssssub;
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY,
        "(sdsssxxa{sas})", &sub);
    vector<jstreams::IndexedDocument>::const_iterator i;
    for (i=s.hits.begin(); i!=s.hits.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        const char* c = i->uri.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        double d = i->score;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_DOUBLE, &d);
        // quick bug fix on what is probably a clucene bug
        // the fragments as stored are sometimes not properly recoded back
        // from usc2 to utf8 which causes dbus to close the connection,
        // whereupon the strigidaemon quits
        if (jstreams::checkUtf8(i->fragment.c_str(), i->fragment.size())) {
            c = i->fragment.c_str();
        } else {
            c = "";
        }
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->mimetype.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->sha1.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        int64_t n = i->size;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT64, &n);
        n = i->mtime;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT64, &n);
        dbus_message_iter_open_container(&ssub, DBUS_TYPE_ARRAY, "{sas})",
            &sssub);
        multimap<string, string>::const_iterator j;
        for (j = i->properties.begin(); j != i->properties.end(); ++j) {
            dbus_message_iter_open_container(&sssub, DBUS_TYPE_DICT_ENTRY, 0,
                &ssssub);
            const char* c = j->first.c_str();
            dbus_message_iter_append_basic(&ssssub, DBUS_TYPE_STRING, &c);
            dbus_message_iter_open_container(&ssssub, DBUS_TYPE_ARRAY, "s",
                &sssssub);
            c = j->second.c_str();
            dbus_message_iter_append_basic(&sssssub, DBUS_TYPE_STRING, &c);
            dbus_message_iter_close_container(&ssssub, &sssssub);
            dbus_message_iter_close_container(&sssub, &ssssub);
        }
        dbus_message_iter_close_container(&ssub, &sssub);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
/** map multimap<int,string> to 'a(is)' **/
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::multimap<int, std::string>& m) {
    DBusMessageIter sub, ssub;
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY, "(is)", &sub);
    multimap<int,string>::const_iterator i;
    for (i = m.begin(); i != m.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        int32_t n = i->first;
        const char* s = i->second.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT32, &n);
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &s);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::vector<std::pair<bool, std::string> >& s) {
    return *this;
}
