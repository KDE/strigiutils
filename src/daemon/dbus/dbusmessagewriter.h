#ifndef DBUSMESSAGEWRITER_H
#define DBUSMESSAGEWRITER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>
#include <set>
#include <map>

class DBusMessageWriter {
    DBusConnection* conn;
    DBusMessage* reply;
    DBusMessage* error;
    DBusMessageIter it;
public:
    DBusMessageWriter(DBusConnection* conn, DBusMessage* msg);
    ~DBusMessageWriter();
    void setError(const std::string& error);
    DBusMessageWriter& operator<<(bool b);
    DBusMessageWriter& operator<<(const std::string& s);
    DBusMessageWriter& operator<<(const std::set<std::string>& s);
    DBusMessageWriter& operator<<(const std::map<std::string, std::string>& s);
};
#endif
