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
#include "dbusserver.h"
#include "dbuscpp/dbushandler.h"
#include "dbusclientinterface.h"
#include "dbusxesamlivesearchinterface.h"
#include "../xesam/xesamlivesearch.h"
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

DBusServer::DBusServer(ClientInterface* iface, XesamLiveSearch* x)
        :StrigiThread("DBusServer"), handler(new DBusHandler()) {
    handler->addBusName("vandenoever.strigi");
    handler->addBusName("org.freedesktop.xesam.searcher");
    // add strigi interface
    interface = new DBusClientInterface("/search",
        handler->connection(), iface);
    handler->addInterface("/search", interface->interface());
    // add xesam interface
    xesam = new DBusXesamLiveSearchInterface(
        "/org/freedesktop/xesam/searcher/main", handler->connection(), x);
    x->addInterface(xesam);
    handler->addInterface("/org/freedesktop/xesam/searcher/main",
        xesam->interface());
}
DBusServer::~DBusServer() {
    delete handler;
    delete interface;
    delete xesam;
}
void*
DBusServer::run(void*) {
    return handler->handle() ?this :0;
}
void
DBusServer::stopThread() {
    cerr << "stop" << endl;
    handler->stop();
}
