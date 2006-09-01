// generated from dbusclientinterface.h by makecode.pl
#include "dbusclientinterface.h"
#include "dbusmessagereader.h"
#include "dbusmessagewriter.h"
#include "../clientinterface.h"
#include <sstream>
DBusClientInterface::DBusClientInterface(ClientInterface* i)
        :DBusObjectInterface("vandenoever.strigi"), impl(i) {
    handlers["getStatus"] = &DBusClientInterface::getStatus;
    handlers["isActive"] = &DBusClientInterface::isActive;
    handlers["getFilteringRules"] = &DBusClientInterface::getFilteringRules;
    handlers["setIndexedDirectories"] = &DBusClientInterface::setIndexedDirectories;
    handlers["getIndexedDirectories"] = &DBusClientInterface::getIndexedDirectories;
    handlers["stopIndexing"] = &DBusClientInterface::stopIndexing;
    handlers["setFilteringRules"] = &DBusClientInterface::setFilteringRules;
    handlers["getHits"] = &DBusClientInterface::getHits;
    handlers["startIndexing"] = &DBusClientInterface::startIndexing;
    handlers["countHits"] = &DBusClientInterface::countHits;
    handlers["stopDaemon"] = &DBusClientInterface::stopDaemon;
}
DBusHandlerResult
DBusClientInterface::handleCall(DBusConnection* connection, DBusMessage* msg){
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
DBusClientInterface::getIntrospectionXML() {
    std::ostringstream xml;
    xml << "  <interface name='"+getInterfaceName()+"'>\n"
    << "    <method name='getStatus'>\n"
    << "      <arg name='out' type='a{ss}' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='isActive'>\n"
    << "      <arg name='out' type='b' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='getFilteringRules'>\n"
    << "      <arg name='out' type='as' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='setIndexedDirectories'>\n"
    << "      <arg name='d' type='as' direction='in'/>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='getIndexedDirectories'>\n"
    << "      <arg name='out' type='as' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='stopIndexing'>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='setFilteringRules'>\n"
    << "      <arg name='rules' type='as' direction='in'/>\n"
    << "    </method>\n"
    << "    <method name='getHits'>\n"
    << "      <arg name='query' type='s' direction='in'/>\n"
    << "      <arg name='max' type='i' direction='in'/>\n"
    << "      <arg name='offset' type='i' direction='in'/>\n"
    << "      <arg name='out' type='a(sdsssxxa{ss})' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='startIndexing'>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='countHits'>\n"
    << "      <arg name='query' type='s' direction='in'/>\n"
    << "      <arg name='out' type='i' direction='out'/>\n"
    << "    </method>\n"
    << "    <method name='stopDaemon'>\n"
    << "      <arg name='out' type='s' direction='out'/>\n"
    << "    </method>\n"
    << "  </interface>\n";
    return xml.str();
}
void
DBusClientInterface::getStatus(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->getStatus();
    }
}
void
DBusClientInterface::isActive(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->isActive();
    }
}
void
DBusClientInterface::getFilteringRules(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->getFilteringRules();
    }
}
void
DBusClientInterface::setIndexedDirectories(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::set<std::string> d;
    reader >> d;
    if (reader.isOk()) {
        writer << impl->setIndexedDirectories(d);
    }
}
void
DBusClientInterface::getIndexedDirectories(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->getIndexedDirectories();
    }
}
void
DBusClientInterface::stopIndexing(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->stopIndexing();
    }
}
void
DBusClientInterface::setFilteringRules(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::set<std::string> rules;
    reader >> rules;
    if (reader.isOk()) {
        impl->setFilteringRules(rules);
    }
}
void
DBusClientInterface::getHits(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::string query;
    int max;
    int offset;
    reader >> query >> max >> offset;
    if (reader.isOk()) {
        writer << impl->getHits(query,max,offset);
    }
}
void
DBusClientInterface::startIndexing(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->startIndexing();
    }
}
void
DBusClientInterface::countHits(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    std::string query;
    reader >> query;
    if (reader.isOk()) {
        writer << impl->countHits(query);
    }
}
void
DBusClientInterface::stopDaemon(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        writer << impl->stopDaemon();
    }
}
