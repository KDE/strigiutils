#ifndef DBUSMESSAGEWRITER_H
#define DBUSMESSAGEWRITER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>
#include <set>
#include <map>
#include <vector>

#include "../clientinterface.h"

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
    DBusMessageWriter& operator<<(int32_t i);
    DBusMessageWriter& operator<<(int64_t i);
    DBusMessageWriter& operator<<(double d);
    DBusMessageWriter& operator<<(const std::string& s);
    DBusMessageWriter& operator<<(const std::set<std::string>& s);
    DBusMessageWriter& operator<<(const std::vector<std::string>& s);
    DBusMessageWriter& operator<<(const std::map<std::string, std::string>& s);
    DBusMessageWriter& operator<<(const ClientInterface::Hits& s);
    DBusMessageWriter& operator<<(const std::multimap<int, std::string>&);
};
#endif
