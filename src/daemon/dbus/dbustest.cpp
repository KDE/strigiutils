#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

bool keeprunning;
pthread_t thread;
pthread_mutex_t lock;

void*
serverthread(void*) {
   /* 
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

    for (int i=0; i<10; ++i) {
        printf("server\n");
    }*/

    bool run;
    do {
        // non blocking read of the next available message
//        dbus_connection_read_write_dispatch(conn, 100);
//        dbus_connection_flush(conn);
        pthread_mutex_lock(&lock);
        run = keeprunning;
        pthread_mutex_unlock(&lock);
    } while (run);
    pthread_exit(0);
}
void
stopserver() {
    pthread_mutex_lock(&lock);
    keeprunning = false;
    pthread_mutex_unlock(&lock);
    pthread_join(thread, 0);
    pthread_mutex_destroy(&lock);
}

void
clientthread() {
    sleep(1);
}

int
main() {
    pthread_mutex_init(&lock, NULL);

    int r = pthread_create(&thread, 0, serverthread, 0);
    clientthread();

    return 0;
}
