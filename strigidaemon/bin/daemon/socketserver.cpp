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

#include "socketserver.h"
#include "interface.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

using namespace std;

#if defined(__APPLE__)
#define SOCKET_NOSIGNAL SO_NOSIGPIPE
#elif defined( MSG_NOSIGNAL )
#define SOCKET_NOSIGNAL MSG_NOSIGNAL
#else
#define SOCKET_NOSIGNAL 0
#endif

void*
SocketServer::run(void*) {
    int sd, newSd;
    size_t len;
    socklen_t addlen;
    struct sockaddr_un sock, work;

    /* create new socket */
    unlink(socketname.c_str());
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("cannot open socket ");
        return false;
    }

    /* set the address */
    len = socketname.length()+1;
    len = (len > sizeof(sock.sun_path)) ?sizeof(sock.sun_path) :len;
    strncpy(sock.sun_path, socketname.c_str(), len);
    sock.sun_path[len] = '\0';

    /* bind server port */
    sock.sun_family = AF_UNIX;
    if (bind(sd, (struct sockaddr *)&sock, sizeof(sock))<0) {
        perror("cannot bind port ");
        return false;
    }

    if (::listen(sd, 5) < 0) {
        perror("cannot listen to port");
        return false;
    }

    while (interface->isActive()) {
        addlen = sizeof(work);
        newSd = accept(sd, (struct sockaddr*)&(work), &addlen);
        if (newSd < 0) {
            break;
        }

        if (!readRequest(newSd)) {
            close(sd);
            continue;
        }
        response.clear();
        handleRequest();
        sendResponse(newSd);
        close(newSd);

        /* the server is now free to accept another socket request */
    }
    if (close(sd) < 0) {
        perror("close socket");
    }
    return &thread;
}
bool
SocketServer::readRequest(int sd) {
    request.clear();
    string line;
    char c;
    while (true) {
        ssize_t r = recv(sd, &c, 1, 0);
        if (r < 0) {
            error = "Error reading from socket: ";
            error += strerror(errno);
            return false;
        } else if (r == 0 || c == 0) {
            if (line.size() > 0) {
                request.push_back(line);
            }
            return true;
        } else if (c == '\n') {
            if (line.size() == 0) {
                // finished reading the request
                return true;
            }
            request.push_back(line);
            line.clear();
        } else {
            line += c;
        }
    }
    return true;
}
bool
SocketServer::sendResponse(int sd) {
    ssize_t r;
    for (uint i=0; i<response.size(); ++i) {
        string line = response[i];
        size_t p = line.find('\n');
        while (p != string::npos) {
            // TODO: give out warning: this line should not contain '\n'
            line[p] = ' ';
            p = line.find('\n', p);
        }
        line += '\n';
        p = 0;
        size_t len = line.length();
        do {
            r = send(sd, line.c_str()+p, len-p, SOCKET_NOSIGNAL);
            if (r < 0) {
                printf("error writing response\n");
                return false;
            }
            p += r;
        } while (p < len);
    }
    r = send(sd, "\n", 1, SOCKET_NOSIGNAL);
    return r > 0;
}
void
SocketServer::handleRequest() {
    response.clear();
    if (request.size() == 4 && request[0] == "query") {
        int max = atoi(request[2].c_str());
        int off = atoi(request[3].c_str());
        ClientInterface::Hits hits = interface->getHits(request[1], max, off);
        response.clear();
        vector<Strigi::IndexedDocument>::const_iterator i;
        ostringstream oss;
        for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
            response.push_back((i->uri.length())?i->uri:" ");
            response.push_back((i->fragment.length())?i->fragment:" ");
            response.push_back((i->mimetype.length())?i->mimetype:" ");
            oss << i->score;
            response.push_back(oss.str());
            oss.str("");
            oss << i->size;
            response.push_back(oss.str());
            oss.str("");
            oss << i->mtime;
            response.push_back(oss.str());
            oss.str("");
            multimap<string, string>::const_iterator j;
            for (j = i->properties.begin(); j != i->properties.end(); ++j) {
                 response.push_back(j->first+':'+j->second);
            }
        }
        return;
    }
    if (request.size() == 2 && request[0] == "countHits") {
        int count = interface->countHits(request[1]);
        response.clear();
        ostringstream c;
        c << count;
        response.push_back(c.str());
        return;
    }
    if (request.size() == 1 && request[0] == "getStatus") {
        map<string, string> status = interface->getStatus();
        map<string,string>::const_iterator i;
        response.clear();
        for (i = status.begin(); i != status.end(); ++i) {
            response.push_back(i->first+':'+i->second);
        }
        return;
    }
    if (request.size() == 1 && request[0] == "stopDaemon") {
        interface->stopDaemon();
        return;
    }
    if (request.size() == 1 && request[0] == "startIndexing") {
        interface->startIndexing();
        return;
    }
    if (request.size() == 1 && request[0] == "stopIndexing") {
        interface->stopIndexing();
        return;
    }
    if (request.size() == 1 && request[0] == "getIndexedDirectories") {
        response.clear();
        set<string> d = interface->getIndexedDirectories();
        set<string>::const_iterator i;
        for (i = d.begin(); i != d.end(); ++i) {
            response.push_back(*i);
        }
        return;
    }
    if (request.size() >= 1 && request[0] == "setIndexedDirectories") {
        set<string> dirs;
        for (uint i=1; i<request.size(); ++i) {
            dirs.insert(request[i]);
        }
        interface->setIndexedDirectories(dirs);
        return;
    }
    if (request.size() == 1 && request[0] == "getIndexedFiles") {
        response.clear();
        set<string> d = interface->getIndexedFiles();
        set<string>::const_iterator i;
        for (i = d.begin(); i != d.end(); ++i) {
            response.push_back(*i);
        }
        return;
    }
    cerr << "unknown request '" << request[0] << "' of size "
        << request.size() << endl;
    response.push_back("error");
    response.push_back("no valid request");
}
