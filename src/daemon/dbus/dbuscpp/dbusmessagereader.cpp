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
#include "dbusmessagereader.h"
using namespace std;

DBusMessageReader::DBusMessageReader(DBusMessage* m) :msg(m), ok(true) {
    // read the arguments
    if (!dbus_message_iter_init(msg, &it)) {
        msg = 0;
    } else {
        dbus_message_ref(msg);
    }
}
DBusMessageReader&
DBusMessageReader::operator>>(dbus_uint32_t& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&it)
            && DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(dbus_int32_t& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(dbus_uint64_t& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_UINT64 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(dbus_int64_t& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_INT64 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(string& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    const char* c;
    dbus_message_iter_get_basic(&it, &c);
    s.assign(c);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(set<string>& s) {
    s.clear();
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_STRING != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    const char* value;
    dbus_message_iter_recurse(&it, &sub);
    while (dbus_message_iter_get_arg_type(&sub) == DBUS_TYPE_STRING) {
        dbus_message_iter_get_basic(&sub, &value);
        s.insert(value);
        dbus_message_iter_next(&sub);
    }
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(vector<string>& s) {
    s.clear();
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_STRING != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    const char* value;
    dbus_message_iter_recurse(&it, &sub);
    while (dbus_message_iter_get_arg_type(&sub) == DBUS_TYPE_STRING) {
        dbus_message_iter_get_basic(&sub, &value);
        s.push_back(value);
        dbus_message_iter_next(&sub);
    }
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(vector<int32_t>& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_INT32 != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    dbus_message_iter_recurse(&it, &sub);
    int length;
    int32_t* array;
    dbus_message_iter_get_fixed_array(&sub, &array, &length);
    s.assign(array, array+length);
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(vector<uint32_t>& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_UINT32 != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    dbus_message_iter_recurse(&it, &sub);
    int length;
    uint32_t* array;
    dbus_message_iter_get_fixed_array(&sub, &array, &length);
    s.assign(array, array+length);
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(vector<char>& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_BYTE != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    dbus_message_iter_recurse(&it, &sub);
    int length;
    char* array;
    dbus_message_iter_get_fixed_array(&sub, &array, &length);
    s.assign(array, array+length);
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(multimap<int, string>& m) {
    if (!isOk()) return *this;
    DBusMessageIter sub;
    DBusMessageIter ssub;
    int32_t n;
    const char* value;
    if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_ARRAY) {
        dbus_message_iter_recurse(&it, &sub);
        while (dbus_message_iter_get_arg_type(&sub) == DBUS_TYPE_STRUCT) {
            dbus_message_iter_recurse(&sub, &ssub);
            if (dbus_message_iter_get_arg_type(&ssub) != DBUS_TYPE_INT32) {
                close();
                return *this;
            }
            dbus_message_iter_get_basic(&ssub, &n);
            dbus_message_iter_next(&ssub);
            if (dbus_message_iter_get_arg_type(&ssub) != DBUS_TYPE_STRING) {
                close();
                return *this;
            }
            dbus_message_iter_get_basic(&ssub, &value);
            m.insert(make_pair<const int,string>(n,value));
            if (!dbus_message_iter_has_next(&sub)) {
                break;
            }
            dbus_message_iter_next(&sub);
        }
    }
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(vector<pair<bool, string> >& m) {
    if (!isOk()) return *this;
    DBusMessageIter sub;
    DBusMessageIter ssub;
    bool n;
    const char* value;
    if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_ARRAY) {
        dbus_message_iter_recurse(&it, &sub);
        while (dbus_message_iter_get_arg_type(&sub) == DBUS_TYPE_STRUCT) {
            dbus_message_iter_recurse(&sub, &ssub);
            if (dbus_message_iter_get_arg_type(&ssub) != DBUS_TYPE_BOOLEAN) {
                close();
                return *this;
            }
            dbus_message_iter_get_basic(&ssub, &n);
            dbus_message_iter_next(&ssub);
            if (dbus_message_iter_get_arg_type(&ssub) != DBUS_TYPE_STRING) {
                close();
                return *this;
            }
            dbus_message_iter_get_basic(&ssub, &value);
            m.push_back(make_pair<bool,string>(n,value));
            if (!dbus_message_iter_has_next(&sub)) {
                break;
            }
            dbus_message_iter_next(&sub);
        }
    }
    dbus_message_iter_next(&it);
    return *this;
}
/*
 * Check if the iterator is positioned at the end of a valid message.
 */
bool
DBusMessageReader::atEnd() {
    // if the message was not empty and the current type == 0 then we are at the end of
    //   a non-empty message
    // if the message was empty and nothing was read then msg == 0 and ok == true
    //   and we are at the end of a valid message
    // in the other cases we are not at the end of a valid message
    return (msg) ?(dbus_message_iter_get_arg_type(&it) == 0) :ok;
}
