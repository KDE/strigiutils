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
#include <strigi/asyncsocket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* workaround for systems without MSG_NOSIGNAL
   on these systems the sending party will receive a SIGPIPE when the receiving
   party closes the connection */
#if defined(__APPLE__)
#define SOCKET_NOSIGNAL SO_NOSIGPIPE
#elif defined( MSG_NOSIGNAL )
#define SOCKET_NOSIGNAL MSG_NOSIGNAL
#else
#define SOCKET_NOSIGNAL 0
#endif

AsyncSocket::AsyncSocket() {
    status = Idle;
}
AsyncSocket::~AsyncSocket() {
    close();
}
void
AsyncSocket::open() {
    struct sockaddr_un  serv_addr;

    // create a socket
    socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket < 0) {
        error = "Could not create socket: ";
        error += strerror(errno);
        return;
    }

    // set the address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    size_t len = socketpath.length();
    len = (len > sizeof(serv_addr.sun_path)) ?sizeof(serv_addr.sun_path) :len;
    strncpy(serv_addr.sun_path, socketpath.c_str(), len);
    serv_addr.sun_path[len] = '\0';

    // connect to the server
    int r = connect(socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (r < 0) {
        error = "Could not connect to server: ";
        error += strerror(errno);
        ::close(socket);
        socket = -1;
    }
}
void
AsyncSocket::close() {
    if (socket >= 0) {
        ::close(socket);
    }
}
bool
AsyncSocket::sendRequest(const std::string& req) {
    if (status == Reading || status == Writing) {
        close();
    }
    open();
    if (socket < 0) {
        status = Error;
        return false;
    }
    error.clear();
    request = req;
    response.clear();
    writepos = 0;
    status = Writing;
    return true;
}
bool
AsyncSocket::statusChanged() {
    if (status == Error || status == Idle) return false;
    if (status == Writing) {
        write();
    }
    if (status == Reading) {
        read();
    }
    return status == Error || status == Idle;
}
void
AsyncSocket::write() {
    ssize_t r = send(socket, request.c_str()+writepos,
        request.length()-writepos, SOCKET_NOSIGNAL);
    if (r != -1) {
        writepos += (unsigned)r;
        if (writepos == request.length()) {
            status = Reading;
        }
    } else {
        switch (errno) {
        case EAGAIN:
            break;
        default:
            status = Error;
            error = strerror(errno);
            break;
        }
    }
}
void
AsyncSocket::read() {
    char c;
    while (true) {
        ssize_t r = recv(socket, &c, 1, MSG_DONTWAIT|SOCKET_NOSIGNAL);
        switch (r) {
        case 0:
            close();
            status = Idle;
            return;
        case 1:
            response += c;
            break;
        default:
            if (errno != EAGAIN) {
                printf("error %i: %s\n", errno, strerror(errno));
                status = Error;
                error = strerror(EAGAIN);
                close();
            }
            return;
        }
    }
}
