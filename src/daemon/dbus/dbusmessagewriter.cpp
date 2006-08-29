#include "dbusmessagewriter.h"
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
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
DBusMessageWriter::operator<<(const std::string& s) {
    const char* c = s.c_str();
    dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &c);
    printf("hi\n");
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
}
