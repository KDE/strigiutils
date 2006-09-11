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
    dbus_message_iter_open_container(&it, DBUS_TYPE_ARRAY,
        "(sdsssxxa{ss})", &sub);
    vector<jstreams::IndexedDocument>::const_iterator i;
    for (i=s.hits.begin(); i!=s.hits.end(); ++i) {
        dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, 0, &ssub);
        const char* c = i->uri.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        double d = i->score;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_DOUBLE, &d);
        c = i->fragment.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->mimetype.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        c = i->sha1.c_str();
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_STRING, &c);
        int64_t n = i->size;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT64, &n);
        n = i->mtime;
        dbus_message_iter_append_basic(&ssub, DBUS_TYPE_INT64, &n);
        dbus_message_iter_open_container(&ssub, DBUS_TYPE_ARRAY, "{ss}",&sssub);
        map<string, string>::const_iterator j;
        for (j = i->properties.begin(); j != i->properties.end(); ++j) {
            dbus_message_iter_open_container(&sssub, DBUS_TYPE_DICT_ENTRY, 0,
                &ssssub);
            c = j->first.c_str();
            dbus_message_iter_append_basic(&ssssub, DBUS_TYPE_STRING, &c);
            c = j->second.c_str();
            dbus_message_iter_append_basic(&ssssub, DBUS_TYPE_STRING, &c);
            dbus_message_iter_close_container(&sssub, &ssssub);
        }
        dbus_message_iter_close_container(&ssub, &sssub);
        dbus_message_iter_close_container(&sub, &ssub);
    }
    dbus_message_iter_close_container(&it, &sub);
    return *this;
}
DBusMessageWriter&
DBusMessageWriter::operator<<(const std::multimap<int, std::string>&) {
    return *this;
}
