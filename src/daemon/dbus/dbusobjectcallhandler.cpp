#include "dbusobjectcallhandler.h"
#include "dbusobjectinterface.h"
#include "dbusmessagewriter.h"
#include <sstream>
using namespace std;

class IntrospectionInterface : public DBusObjectInterface {
private:
    DBusObjectCallHandler* object;
    DBusHandlerResult handleCall(DBusConnection* connection, DBusMessage* msg);
    void introspect(DBusMessage* msg, DBusConnection* conn);
public:
    IntrospectionInterface(DBusObjectCallHandler* o)
        : DBusObjectInterface("org.freedesktop.DBus.Introspectable"),
          object(o) {}
    ~IntrospectionInterface() {}
    std::string getIntrospectionXML();
};
std::string
IntrospectionInterface::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "  <interface name='"+getInterfaceName()+"'>\n"
    << "    <method name='Introspect'>\n"
    << "      <arg name='xml_data' type='s' direction='out'/>\n"
    << "    </method>\n  </interface>\n";
    return xml.str();
}
DBusHandlerResult
IntrospectionInterface::handleCall(DBusConnection* conn, DBusMessage* msg) {
    const char* i = getInterfaceName().c_str();
    if (dbus_message_is_method_call(msg, i, "Introspect")) {
        introspect(msg, conn);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
void
IntrospectionInterface::introspect(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageWriter writer(conn, msg);
    writer << object->getIntrospectionXML();
}

const DBusObjectPathVTable DBusObjectCallHandler::vtable = {
    NULL,
    message_function,
    NULL
};
DBusObjectCallHandler::DBusObjectCallHandler(const std::string& path)
        : objectpath(path) {
    introspection = new IntrospectionInterface(this);
    addInterface(introspection);
}
DBusObjectCallHandler::~DBusObjectCallHandler() {
    delete introspection;
}
void
DBusObjectCallHandler::addInterface(DBusObjectInterface* interface) {
    interfaces[interface->getInterfaceName()] = interface;
}
DBusHandlerResult
DBusObjectCallHandler::message_function(DBusConnection* connection,
        DBusMessage* msg, void* object) {
    DBusObjectCallHandler* handler
        = static_cast<DBusObjectCallHandler*>(object);
    return handler->handleCall(connection, msg);
}
DBusHandlerResult
DBusObjectCallHandler::handleCall(DBusConnection* connection, DBusMessage* msg){
    const char* iface = dbus_message_get_interface(msg);
    map<string, DBusObjectInterface*>::const_iterator i;
    if (iface == 0) { // no interface was specified, try all interfaces
        DBusHandlerResult r;
        for (i=interfaces.begin(); i!=interfaces.end(); ++i) {
            r = i->second->handleCall(connection, msg);
            if (r == DBUS_HANDLER_RESULT_HANDLED) {
                return r;
            }
        }
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    } else {
        i = interfaces.find(dbus_message_get_interface(msg));
        if (i == interfaces.end()) {
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
        return i->second->handleCall(connection, msg);
    }
}
void
DBusObjectCallHandler::registerOnConnection(DBusConnection* c) {
    dbus_connection_register_object_path(c, objectpath.c_str(),
        &DBusObjectCallHandler::vtable, this);
}
std::string
DBusObjectCallHandler::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "<node name='" << objectpath << "'>\n";
    map<string, DBusObjectInterface*>::const_iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i) {
        xml << i->second->getIntrospectionXML();
    }
    xml << "</node>\n";
    return xml.str();
}
