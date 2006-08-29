#include "dbusserver.h"
#include "interface.h"
#include "dbusmessagereader.h"
#include "dbusmessagewriter.h"
using namespace std;

class StrigiDBusServer {
private:
    Interface* iface;
    static DBusHandlerResult message_function(DBusConnection* connection,
        DBusMessage* message, void* user_data);
    DBusHandlerResult handleCall(DBusConnection* connection, DBusMessage* msg);
    typedef void (StrigiDBusServer::*handlerFunction)
        (DBusMessage* msg, DBusConnection* conn);
    map<string, handlerFunction> handlers;

    void countHits(DBusMessage* msg, DBusConnection* conn);
    void getHits(DBusMessage* msg, DBusConnection* conn);
    void getStatus(DBusMessage* msg, DBusConnection* conn);
    void stopDaemon(DBusMessage* msg, DBusConnection* conn);
public:
    static const DBusObjectPathVTable vtable;
    static const char* servicename;
    static const char* interface;
    static const char* objectpath;

    StrigiDBusServer(Interface* interface);
    ~StrigiDBusServer() {}
};

const char* StrigiDBusServer::servicename = "vandenoever.strigi";
const char* StrigiDBusServer::interface = "vandenoever.strigi";
const char* StrigiDBusServer::objectpath = "/search";

const DBusObjectPathVTable StrigiDBusServer::vtable = {
    NULL,
    message_function,
    NULL
};

StrigiDBusServer::StrigiDBusServer(Interface* i) :iface(i) {
    handlers["countHits"] = &StrigiDBusServer::countHits;
    handlers["getHits"] = &StrigiDBusServer::getHits;
    handlers["stopDaemon"] = &StrigiDBusServer::stopDaemon;
    handlers["getStatus"] = &StrigiDBusServer::getStatus;
}
DBusHandlerResult
StrigiDBusServer::message_function(DBusConnection* connection,
    DBusMessage* msg, void* object) {

    StrigiDBusServer* server = static_cast<StrigiDBusServer*>(object);
    if (strcmp(dbus_message_get_interface(msg), interface) == 0) {
        return server->handleCall(connection, msg);
    }
    printf("could not find interface %s\n", dbus_message_get_interface(msg));
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
DBusHandlerResult
StrigiDBusServer::handleCall(DBusConnection* connection, DBusMessage* msg) {
    map<string, handlerFunction>::const_iterator h;
    for (h = handlers.begin(); h != handlers.end(); ++h) {
        if (dbus_message_is_method_call(msg, interface, h->first.c_str())) {
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
StrigiDBusServer::countHits(DBusMessage* msg, DBusConnection* conn) {
}
void
StrigiDBusServer::stopDaemon(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        iface->stopDaemon();
    } else {
        //writer.setError("wrong parameters");
    }
}
void
StrigiDBusServer::getHits(DBusMessage* msg, DBusConnection* conn) {
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
StrigiDBusServer::getStatus(DBusMessage* msg, DBusConnection* conn) {
    DBusMessageReader reader(msg);
    DBusMessageWriter writer(conn, msg);
    if (reader.isOk()) {
        printf("get status\n");
        writer << iface->getStatus();
    } else {
        //writer.setError("wrong parameters");
    }
}

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
    ret = dbus_bus_request_name(conn, StrigiDBusServer::servicename,
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
    StrigiDBusServer server(interface);

    // register the strigi object
    dbus_connection_register_object_path (conn, StrigiDBusServer::objectpath,
        &StrigiDBusServer::vtable, &server); 
 
    // loop, testing for new messages
    while (interface->isActive()) {
        // non blocking read of the next available message
        dbus_connection_read_write_dispatch(conn, 10);
        dbus_connection_flush(conn);
    }
    printf("hey!\n");

    // close the connection
    dbus_connection_close(conn);
    return true;
}
