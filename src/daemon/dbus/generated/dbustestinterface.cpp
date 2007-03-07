// generated from dbustestinterface.h by makecode.pl
#include "dbustestinterface.h"
#include "dbusmessagereader.h"
#include "dbusmessagewriter.h"
#include <testinterface.h>
#include <sstream>
DBusTestInterface::DBusTestInterface(TestInterface* i)
        :DBusObjectInterface("vandenoever.dbustest.DBusTest"), impl(i) {
    handlers["upload"] = &DBusTestInterface::upload;
    handlers["concat"] = &DBusTestInterface::concat;
    handlers["helloWorld"] = &DBusTestInterface::helloWorld;
    handlers["giveMap"] = &DBusTestInterface::giveMap;
}
DBusHandlerResult
DBusTestInterface::handleCall(DBusConnection* connection, DBusMessage* msg){
    std::map<std::string, handlerFunction>::const_iterator h;
    const char* i = getInterfaceName().c_str();
    for (h = handlers.begin(); h != handlers.end(); ++h) {
        if (dbus_message_is_method_call(msg, i, h->first.c_str())) {
            (this->*h->second)(msg, connection);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
std::string
DBusTestInterface::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "  <interface name='"+getInterfaceName()+"'>\n"
    << "    <method name='upload'>\n"
    << "      <arg name='upload' type='ay' direction='in'/>\n"
    << "      <arg name='out' type='b' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='concat'>\n"
    << "      <arg name='a' type='s' direction='in'/>\n"
    << "      <arg name='b' type='s' direction='in'/>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='helloWorld'>\n"
    << "      <arg name='name' type='s' direction='in'/>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='giveMap'>\n"
    << "      <arg name='out' type='a{ss}' direction='out'/>\n"
    << "    </method>\n"
    << "  </interface>\n";
    return xml.str();
}
void
DBusTestInterface::upload(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::vector<char> upload;
    reader >> upload;
    if (reader.isOk()) {
        writer << impl->upload(upload);
    }
}
void
DBusTestInterface::concat(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::string a;
    std::string b;
    reader >> a >> b;
    if (reader.isOk()) {
        writer << impl->concat(a,b);
    }
}
void
DBusTestInterface::helloWorld(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::string name;
    reader >> name;
    if (reader.isOk()) {
        writer << impl->helloWorld(name);
    }
}
void
DBusTestInterface::giveMap(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->giveMap();
    }
}
