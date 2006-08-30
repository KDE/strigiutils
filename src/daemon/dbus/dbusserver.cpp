#include "dbusserver.h"
#include "interface.h"
#include "dbusobjectcallhandler.h"
#include "dbusclientinterface.h"
using namespace std;
/*
class StrigiDBusInterface : public DBusObjectInterface {
private:
    typedef void (StrigiDBusInterface::*handlerFunction)
        (DBusMessage* msg, DBusConnection* conn);

    Interface* iface;
    map<string, handlerFunction> handlers;

    DBusHandlerResult handleCall(DBusConnection* connection, DBusMessage* msg);

    void countHits(DBusMessage* msg, DBusConnection* conn);
    void getHits(DBusMessage* msg, DBusConnection* conn);
    void getStatus(DBusMessage* msg, DBusConnection* conn);
    void stopDaemon(DBusMessage* msg, DBusConnection* conn);
public:
    StrigiDBusInterface(const std::string& name, Interface* interface);
    ~StrigiDBusInterface() {}
};

StrigiDBusInterface::StrigiDBusInterface(const std::string& name, Interface* i)
        : DBusObjectInterface(name), iface(i) {
    handlers["countHits"] = &StrigiDBusInterface::countHits;
    handlers["getHits"] = &StrigiDBusInterface::getHits;
    handlers["stopDaemon"] = &StrigiDBusInterface::stopDaemon;
    handlers["getStatus"] = &StrigiDBusInterface::getStatus;
}
DBusHandlerResult
StrigiDBusInterface::handleCall(DBusConnection* connection, DBusMessage* msg) {
    map<string, handlerFunction>::const_iterator h;
    const char* i = getInterfaceName().c_str();
    for (h = handlers.begin(); h != handlers.end(); ++h) {
        if (dbus_message_is_method_call(msg, i, h->first.c_str())) {
            (this->*h->second)(msg, connection);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }
    printf("could not find method\n");
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
// in: string
// out: int
void
StrigiDBusInterface::countHits(DBusMessage* msg, DBusConnection* conn) {
}
void
StrigiDBusInterface::stopDaemon(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        iface->stopDaemon();
    } else {
        //writer.setError("wrong parameters");
    }
}
void
StrigiDBusInterface::getHits(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    string a1;
    int32_t a2;
    int32_t a3;
    reader >> a1 >> a2 >> a3;
    if (reader.isOk()) {
        iface->getHits(a1, a2, a3);
    } else {
        //writer.setError("wrong parameters");
    }
}
void
StrigiDBusInterface::getStatus(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        printf("get status\n");
        writer << iface->getStatus();
    } else {
        //writer.setError("wrong parameters");
    }
}
*/
bool
DBusServer::listen() {
    DBusConnection* conn;
    DBusError err;
    int ret;

    printf("Listening for method calls\n");

    // initialise the error
    dbus_error_init(&err);

    // connect to the bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        return false;
    }
    if (NULL == conn) {
        fprintf(stderr, "Connection Null\n");
        return false;
    }

    // request our name on the bus and check for errors
    ret = dbus_bus_request_name(conn, "vandenoever.strigi",
        DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        fprintf(stderr, "Not Primary Owner (%d)\n", ret);
        return false;
    }

    // register the introspection interface
    DBusObjectCallHandler callhandler("/search");
    callhandler.registerOnConnection(conn);

    DBusClientInterface searchinterface(interface);
    callhandler.addInterface(&searchinterface);

//    printf("%s\n", callhandler.getIntrospectionXML().c_str());
 
    // loop, testing for new messages
    while (interface->isActive()) {
        // non blocking read of the next available message
        dbus_connection_read_write_dispatch(conn, -1);
        dbus_connection_flush(conn);
    }

    // close the connection
    dbus_connection_close(conn);
    return true;
}
