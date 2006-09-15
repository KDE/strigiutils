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
#include "filters.h"
#include "interface.h"
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <assert.h>
using namespace std;

#if defined(__APPLE__)
#define SOCKET_NOSIGNAL SO_NOSIGPIPE
#elif defined( unix )
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
            perror("cannot accept connection ");
            return false;
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
        int r = recv(sd, &c, 1, 0);
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
        assert(line.find('\n') == string::npos);
        line += '\n';
        int p = 0;
        int len = line.length();
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
        vector<jstreams::IndexedDocument>::const_iterator i;
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
            map<string, string>::const_iterator j;
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
    if (request.size() == 1 && request[0] == "getFilteringRules") {
        response.clear();
        multimap<int,string> d = interface->getFilteringRules();
        multimap<int,string>::iterator lb, ub, i;
        char buffer[500];
        
        lb = d.lower_bound(PathFilter::RTTI);
        ub = d.upper_bound(PathFilter::RTTI);
        
        // response structure:
        // LINE 0: FILTER_RTTI-TOT_RULES_NUMBER
        // ... rules ...
        // LINE $RULES_NUMBER: FILTER_RTTI-TOT_RULES_NUMBER
        // ... rules ...
        
        snprintf (buffer, 500*sizeof(char), "%i-%u",PathFilter::RTTI, d.count(PathFilter::RTTI));
        response.push_back(string(buffer));
        for (i = lb; i != ub; ++i) {
            response.push_back(i->second);
        }
        
        lb = d.lower_bound(PatternFilter::RTTI);
        ub = d.upper_bound(PatternFilter::RTTI);
        
        snprintf (buffer, 500*sizeof(char), "%i-%u",PatternFilter::RTTI, d.count(PatternFilter::RTTI));
        response.push_back(string(buffer));
        for (i = lb; i != ub; ++i) {
            response.push_back(i->second);
        }
        
        return;
    }
    if (request.size() >= 1 && request[0] == "setFilteringRules") {
        // response structure:
        // LINE 0: FILTER_RTTI-TOT_RULES_NUMBER
        // ... rules ...
        // LINE $RULES_NUMBER: FILTER_RTTI-TOT_RULES_NUMBER
        // ... rules ...
        multimap<int,string> rules;
        int filterRTTI = 0;
        unsigned int filterNum = 0;
        int unsigned i = 1;
    
        while ((i < request.size()) && (sscanf(request[i].c_str(), "%i-%u", &filterRTTI, &filterNum) != 0))
        {
            i++;
            for (unsigned int ub = i + filterNum; (i < ub) && (i < request.size()); i++)
                rules.insert (make_pair(filterRTTI, request[i]));
        }
        
        interface->setFilteringRules(rules);
        return;
    }
    printf("unknown request '%s' of size %i\n",
        request[0].c_str(), request.size());
    response.push_back("error");
    response.push_back("no valid request");
}
