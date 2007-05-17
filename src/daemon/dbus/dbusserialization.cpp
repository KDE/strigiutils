#include "dbusserialization.h"
#include "dbusmessagewriter.h"
#include "textutils.h"
using namespace std;

/* yes this is ugly, i have to change the interface */
DBusMessageWriter&
operator<<(DBusMessageWriter& w,
        const ClientInterface::Hits& s) {
    DBusMessageIter sub;
    DBusMessageIter ssub;
    DBusMessageIter sssub;
    DBusMessageIter ssssub;
    DBusMessageIter sssssub;
    dbus_message_iter_open_container(&w.it, DBUS_TYPE_ARRAY,
        "(sdsssxxa{sas})", &sub);
    vector<Strigi::IndexedDocument>::const_iterator i;
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
        if (Strigi::checkUtf8(i->fragment.c_str(), i->fragment.size())) {
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
    dbus_message_iter_close_container(&w.it, &sub);
    return w;
}

DBusMessageWriter&
operator<<(DBusMessageWriter& w, const Variant& v) {
    return w;
}
DBusMessageWriter&
operator<<(DBusMessageWriter& w, const vector<vector<Variant> >& v) {
    return w;
}
DBusMessageReader&
operator>>(DBusMessageReader& r, const Variant& v) {
}
