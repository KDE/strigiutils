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
    if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(time_t& s) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&it)) {
        close();
        return *this;
    }
    dbus_message_iter_get_basic(&it, &s);
    dbus_message_iter_next(&it);
    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(int32_t& s) {
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
DBusMessageReader::operator>>(int64_t& s) {
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
DBusMessageReader::operator>>(std::string& s) {
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
    dbus_message_iter_recurse(&it, &sub);
    const char* value;
    do {
        dbus_message_iter_get_basic(&sub, &value);
        s.insert(value);
    } while(dbus_message_iter_next(&sub));
    dbus_message_iter_next(&it);
   
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
    s.assign(array, array+length);
    dbus_message_iter_next(&it);

    return *this;
}
DBusMessageReader&
DBusMessageReader::operator>>(std::multimap<int, std::string>& m) {
    if (!isOk()) return *this;
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&it)
        || DBUS_TYPE_STRUCT != dbus_message_iter_get_element_type(&it)) {
        close();
        return *this;
    }
    DBusMessageIter sub;
    DBusMessageIter ssub;
    dbus_message_iter_recurse(&it, &sub);
    int32_t n;
    const char* value;
    do {
        // this probably doesnt work without opening each struct for reading
        dbus_message_iter_recurse(&sub, &ssub);
        dbus_message_iter_get_basic(&sub, &n);
        dbus_message_iter_get_basic(&sub, &value);
        m.insert(make_pair(n,value));
    } while(dbus_message_iter_next(&sub));
    dbus_message_iter_next(&it);
    return *this;
}
