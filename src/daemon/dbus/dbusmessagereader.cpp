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
DBusMessageReader::operator>>(int32_t s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(std::string& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    const char* c;
    dbus_message_iter_get_basic(&it, &c);
    s.assign(c);
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
    dbus_message_iter_recurse(&it, &sub);
    const char* value;
    do {
        dbus_message_iter_get_basic(&sub, &value);
        s.insert(value);
    } while(dbus_message_iter_next(&sub));
    
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(std::vector<char>& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_BYTE != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }

    DBusMessageIter sub;
    dbus_message_iter_recurse(&it, &sub);
    int length = dbus_message_iter_get_array_len(&sub);
    char* array;
    dbus_message_iter_get_fixed_array(&sub, &array, &length);
//    s.assign(array, array+length);

    return *this;
}
