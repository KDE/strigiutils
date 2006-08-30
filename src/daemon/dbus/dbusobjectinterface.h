#ifndef DBUSOBJECTINTERFACE_H
#define DBUSOBJECTINTERFACE_H

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <string>

class DBusObjectInterface {
private:
    const std::string interfacename;
public:
    DBusObjectInterface(const std::string& i) :interfacename(i) {}
    virtual ~DBusObjectInterface() {};
    virtual DBusHandlerResult handleCall(DBusConnection* connection,
        DBusMessage* msg) = 0;
    virtual std::string getIntrospectionXML() = 0;
    const std::string& getInterfaceName() const {
        return interfacename;
    }
};

#endif
