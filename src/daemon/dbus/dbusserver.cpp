#include "dbusserver.h"
#include "interface.h"
#include "dbusobjectcallhandler.h"
#include "dbusclientinterface.h"
#include "testinterface.h"
#include "dbustestinterface.h"
using namespace std;

void*
DBusServer::run(void*) {
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

    TestInterface test;
    DBusTestInterface testinterface(&test);
    callhandler.addInterface(&testinterface);

//    printf("%s\n", callhandler.getIntrospectionXML().c_str());
 
    // loop, testing for new messages
    while (interface->isActive() && getState() != Stopping) {
        // non blocking read of the next available message
        dbus_connection_read_write_dispatch(conn, -1);
        dbus_connection_flush(conn);
    }

    // close the connection
    dbus_connection_close(conn);
    return &thread;
}
