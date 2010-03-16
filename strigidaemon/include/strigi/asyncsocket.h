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
#ifndef ASYNCSOCKET_H
#define ASYNCSOCKET_H

#include "strigi/strigiconfig.h"
#include <string>

class SEARCHCLIENT_EXPORT AsyncSocket {
public:
    enum Status { Idle, Writing, Reading, Error };
private:
    Status status;
    int socket;
    unsigned writepos;
    std::string socketpath;
    std::string error;
    std::string request;
    std::string response;

    void open();
    void close();
    void read();
    void write();
public:
    AsyncSocket();
    void setSocketPath(const std::string& path) {
        socketpath = path;
    }
    ~AsyncSocket();
    bool statusChanged();
    bool sendRequest(const std::string& req);
    const std::string& getResponse() const { return response; }
    Status getStatus() const { return status; }
    const std::string& getError() const { return error; }
};

#endif
