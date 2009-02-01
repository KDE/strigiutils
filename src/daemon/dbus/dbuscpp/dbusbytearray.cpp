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
#include "dbusbytearray.h"

DBusByteArray::DBusByteArray(DBusMessage* m) :msg(0), array(0), length(0) {
    // try to read an array from the message

    // check that the arguments are ok
    DBusMessageIter it;
    if (!dbus_message_iter_init(msg, &it)) return;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)) return;
    if (DBUS_TYPE_BYTE != dbus_message_iter_get_element_type(&it)) return;

    // get pointers to the data
    DBusMessageIter sub;
    dbus_message_iter_recurse(&it, &sub);
    length = dbus_message_iter_get_array_len(&sub);
    dbus_message_iter_get_fixed_array(&sub, &array, &length);

    // reference the pointer
    msg = m;
    dbus_message_ref(msg);
}

