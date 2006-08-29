#ifndef DBUSBYTEARRAY_H
#define DBUSBYTEARRAY_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>

class DBusByteArray {
private:
    DBusMessage* msg;
    const char* array;
    int32_t length;
public:
    DBusByteArray(DBusMessage* msg);
    ~DBusByteArray() {
        if (msg) {
            dbus_message_unref(msg);
        }
    }
    const char* getArray() const {
        return array;
    }
    int32_t getLength() const {
        return length;
    }
};

#endif
