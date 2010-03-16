/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <dbus/dbus.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include "strigi/strigi_thread.h"

bool keeprunning;
STRIGI_THREAD_DEFINE(thread);
STRIGI_MUTEX_DEFINE(lock);
DBusConnection* conn;
int quitpipe[2];

void*
serverthread(void*) {
    DBusMessage* msg;
    DBusError err;
    int ret;

    printf("Listening for method calls\n");
    if (pipe(quitpipe) == -1) {
        fprintf(stderr, "Could not create pipe.\n");
        return 0;
    }

    // initialise the error
    dbus_error_init(&err);

    // connect to the bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        return 0;
    }
    if (NULL == conn) {
        fprintf(stderr, "Connection Null\n");
        return 0;
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
    }
    int fd;
    dbus_connection_get_unix_fd(conn, &fd);

    bool run;
    do {
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
        dbus_connection_read_write_dispatch(conn, 0);
//        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);
        dbus_connection_flush(conn);
        dbus_message_unref(msg);

        STRIGI_MUTEX_LOCK(&lock);
        run = keeprunning;
        STRIGI_MUTEX_UNLOCK(&lock);
    } while (run);

    dbus_connection_unref(conn);
    dbus_connection_unref(conn);
    dbus_shutdown();
    printf("after dbus_shutdown\n");
    return 0;
}
void
startserver() {
    STRIGI_MUTEX_INIT(&lock);
    STRIGI_THREAD_CREATE(&thread, serverthread, 0);
    keeprunning = true;
}
void
stopserver() {
    STRIGI_MUTEX_LOCK(&lock);
    keeprunning = false;
    STRIGI_MUTEX_UNLOCK(&lock);
    close(quitpipe[1]);

    printf("before join\n");
    STRIGI_THREAD_JOIN(thread);
    printf("after join\n");
    STRIGI_MUTEX_DESTROY(&lock);
    printf("end of stopserver\n");
}

void
clientthread() {
    printf("before sleep\n");
    sleep(1);
    printf("after sleep\n");
}

int
main() {
    startserver();

    // run client code
    clientthread();

    // stop server
    stopserver();

    return 0;
}
