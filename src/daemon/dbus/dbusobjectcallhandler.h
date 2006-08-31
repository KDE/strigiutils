#ifndef DBUSOBJECTCALLHANDLER_H
#define DBUSOBJECTCALLHANDLER_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>
#include <map>

class DBusObjectInterface;

class DBusObjectCallHandler {
private:
    static DBusHandlerResult message_function(DBusConnection* connection,
        DBusMessage* message, void* user_data);
    DBusHandlerResult handleCall(DBusConnection* connection, DBusMessage* msg);
    DBusObjectInterface* introspection;

    const std::string objectpath;
    std::map<std::string, DBusObjectInterface*> interfaces;
public:
    static const DBusObjectPathVTable vtable;
    explicit DBusObjectCallHandler(const std::string& path);
    ~DBusObjectCallHandler();
    void addInterface(DBusObjectInterface* interface);
    void registerOnConnection(DBusConnection* c);
    std::string getIntrospectionXML();
};

#endif
