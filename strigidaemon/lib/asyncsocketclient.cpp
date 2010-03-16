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
#include <strigi/asyncsocketclient.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

bool
AsyncSocketClient::statusChanged() {
    if (method.length() == 0 || !socket.statusChanged()) {
        return false;
    }
    if (method == "countHits") {
        handleCountHitsResponse();
    }
    else if (method == "query") {
        handleQueryResponse();
    }
    else if (method == "getIndexedDirectories") {
        handleGetIndexedDirectoriesResponse();
    }
    else if (method == "getStatus") {
        handleGetDaemonStatusResponse();
    }
    method.clear();
    return true;
}
vector<string>
AsyncSocketClient::splitResponse() const {
    vector<string> response;
    const char* p = socket.getResponse().c_str();
    string line;

    while (true)
    {
        if ((*p != '\n') && (*p != '\0'))
            line += *p;
        else if (line.length() > 0)
        {
            response.push_back (line);
            line.clear();
        }

        p++;

        if (*p == '\0')
            break;
    }

    return response;
}
bool
AsyncSocketClient::countHits(const std::string& query) {
    method = "countHits";
    string msg = method+'\n'+query+"\n\n";
    return socket.sendRequest(msg);
}
void
AsyncSocketClient::handleCountHitsResponse() {
    if (socket.getStatus() == AsyncSocket::Error) {
        hitcount = -1;
        return;
    }
    istringstream i(socket.getResponse());
    i >> hitcount;
}
bool
AsyncSocketClient::query(const std::string& query, int max, int off) {
    method = "query";
    ostringstream oss;
    oss << method << "\n" << query << "\n" << max << "\n" << off << "\n\n";
    return socket.sendRequest(oss.str());
}
void
AsyncSocketClient::handleQueryResponse() {
    hits.hits.clear();
    if (socket.getStatus() == AsyncSocket::Error) {
        return;
    }
    vector<string> response(splitResponse());
    unsigned i = 0;
    while (i+6 < response.size()) {
        Strigi::IndexedDocument h;
        h.uri = response[i++];
        h.fragment = response[i++];
        h.mimetype = response[i++];
        h.score = (float)atof(response[i++].c_str());
        h.size = atoi(response[i++].c_str());
        h.mtime = atoi(response[i++].c_str());
        while (i < response.size()) {
            const char* s = response[i].c_str();
            const char* v = strchr(s, ':');
            if (!v) break;
            const char* d = strchr(s, '/');
            if (d && d < v) break;
            string n(s, v-s);
            h.properties.insert(make_pair<const string, string>(n,v+1));
            ++i;
        }
        hits.hits.push_back(h);
    }
}
bool
AsyncSocketClient::getIndexedDirectories() {
    method = "getIndexedDirectories";
    string message;
    message = method + "\n\n";
    return socket.sendRequest(message);
}
void
AsyncSocketClient::handleGetIndexedDirectoriesResponse() {
    indexedDirs.clear();
    if (socket.getStatus() == AsyncSocket::Error) {
        return;
    }

    indexedDirs = splitResponse();
}
bool
AsyncSocketClient::getDaemonStatus() {
    method = "getStatus";
    string message;
    message = method + "\n\n";
    return socket.sendRequest(message);
}
void
AsyncSocketClient::handleGetDaemonStatusResponse() {
    daemonStatus.clear();
    if (socket.getStatus() == AsyncSocket::Error) {
        return;
    }
    vector<string> response = splitResponse();
    for (unsigned i=0; i<response.size(); ++i) {
        string s = response[i];
        string::size_type p = s.find(":");
        if (p == string::npos) {
            daemonStatus.clear();
            daemonStatus["error"] = "Communication error.";
            return;
        }
        daemonStatus[s.substr(0,p)] = s.substr(p+1);
    }
}

