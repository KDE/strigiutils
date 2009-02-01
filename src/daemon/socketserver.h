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
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <string>
#include <vector>
#include "strigithread.h"

class Interface;
class SocketServer : public StrigiThread {
private:
    Interface* interface;
    std::string socketname;
    std::string error;
    std::vector<std::string> request;
    std::vector<std::string> response;

    bool readRequest(int sd);
    bool sendResponse(int sd);
    void handleRequest();

    void* run(void*);
public:
    SocketServer(Interface* i) :StrigiThread("SocketServer"), interface(i) {}
    void setSocketName(const std::string& name) {
        socketname = name;
    }
    bool listen() { return run(0); }
};

#endif
