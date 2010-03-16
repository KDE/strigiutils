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
#ifndef ASYNCSOCKETCLIENT_H
#define ASYNCSOCKETCLIENT_H

#include "asyncsocket.h"
#include "clientinterface.h"

class SEARCHCLIENT_EXPORT AsyncSocketClient {
private:
    AsyncSocket socket;
    std::string method;
    std::vector<std::string> indexedDirs;
    std::map<std::string, std::string> daemonStatus;
    int hitcount;
    ClientInterface::Hits hits;

    void handleQueryResponse();
    void handleCountHitsResponse();
    void handleGetIndexedDirectoriesResponse();
    void handleGetDaemonStatusResponse();

    std::vector<std::string> splitResponse() const;
public:
    void setSocketPath(const std::string& path) {
        socket.setSocketPath(path);
    }
    bool countHits(const std::string& query);
    int getHitCount() const { return hitcount; }
    bool query(const std::string& query, int max, int off);
    const ClientInterface::Hits& getHits() const {
        return hits;
    }
    bool getIndexedDirectories();
    std::vector<std::string> getIndexedDirectoriesResponse() const {
        return indexedDirs;
    }
    bool getDaemonStatus();
    std::map<std::string, std::string> getDaemonStatusResponse() const {
        return daemonStatus;
    }
    bool statusChanged();
};

#endif
