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
#include "dbushandler.h"
#include "dbusclientinterface.h"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

DBusServer::DBusServer(ClientInterface* iface)
        :StrigiThread("DBusServer"), handler(new DBusHandler()) {
    interface = new DBusClientInterface("", handler->connection(), iface);
    handler->addInterface(interface->interface());
}
DBusServer::~DBusServer() {
    delete handler;
    delete interface;
}
void*
DBusServer::run(void*) {
    return handler->handle() ?this :0;
}
void
DBusServer::stopThread() {
    handler->stop();
}
