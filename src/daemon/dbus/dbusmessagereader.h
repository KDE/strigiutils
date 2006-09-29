#ifndef DBUSMESSAGEREADER_H
#define DBUSMESSAGEREADER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <set>
#include <map>
#include <string>
#include <vector>

class DBusMessageReader {
private:
    DBusMessage* msg;
    DBusMessageIter it;
    bool ok;

    void close() {
        if (msg) {
            dbus_message_unref(msg);
            msg = 0;
        }
        ok = false;
    }
public:
    explicit DBusMessageReader(DBusMessage* msg);
    ~DBusMessageReader() {
        close();
    }
    DBusMessageReader& operator>>(std::set<std::string>& s);
    DBusMessageReader& operator>>(std::vector<char>& s);
    DBusMessageReader& operator>>(std::string& s);
    DBusMessageReader& operator>>(dbus_uint32_t& s);
    DBusMessageReader& operator>>(int32_t& s);
    DBusMessageReader& operator>>(dbus_uint64_t& s);
    DBusMessageReader& operator>>(int64_t& s);
    DBusMessageReader& operator>>(std::multimap<int, std::string>&);
    DBusMessageReader& operator>>(time_t&);
    bool isOk() const { return ok; }
};

#endif
