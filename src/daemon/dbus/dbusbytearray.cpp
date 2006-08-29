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

