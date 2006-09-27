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
    DBusMessage* msg;
    DBusError err;
    int ret;

    printf("Listening for method calls\n");
    pipe(quitpipe);

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
    if (interface) {
        callhandler.addInterface(&searchinterface);
    }

    TestInterface test;
    DBusTestInterface testinterface(&test);
    callhandler.addInterface(&testinterface);

    //printf("%s\n", callhandler.getIntrospectionXML().c_str());

    // loop, testing for new messages
    while ((!interface || interface->isActive()) && getState() != Stopping) {
        // blocking read of the next available message
        dbus_connection_read_write_dispatch(conn, -1);
        dbus_connection_flush(conn);
    }
/*    int fd;
    dbus_connection_get_unix_fd(conn, &fd);
    bool run;
    do {
        printf("wait for call\n");
        // block until there's activit on the port
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        FD_SET(quitpipe[0], &rfds);
        printf("server\n");
        tv.tv_sec = 50000;
        tv.tv_usec = 0;
        int retval = select(fd+1, &rfds, 0, 0, &tv);
        if (retval == -1 || FD_ISSET(quitpipe[0], &rfds)) break;

        // non blocking read of the next available message
//        dbus_connection_read_write_dispatch(conn, 0);
        dbus_connection_read_write(conn, 1000);
        msg = dbus_connection_pop_message(conn);
        if (msg) {
 
            printf("before handling call %s %s %s\n", dbus_message_get_path(msg), dbus_message_get_interface(msg), dbus_message_get_member(msg));
            callhandler.handleCall(conn, msg);
            printf("after handling call\n");

            dbus_connection_flush(conn);
            dbus_message_unref(msg);
            printf("finished with call\n");
        }

    } while ((!interface || interface->isActive()) && getState() != Stopping);
*/
    // close the connection
    dbus_connection_unref(conn);
    dbus_shutdown();
    return &thread;
}
