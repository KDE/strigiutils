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
#include "queryexecuter.h"
#include "socketclient.h"
#include <QtCore/QDebug>
using namespace std;

void
QueryExecuter::run() {
    return;
    // small delay to avoid querying incomplete queries
    sleep(1);
    mutex.lock();
    QString q = querystring;
    mutex.unlock();

    SocketClient client;
    std::string socket = getenv("HOME");
    socket += "/.strigi/socket";
    client.setSocketName(socket.c_str());
    ClientInterface::Hits hits;

    QString oldq;
    do {
        if (q.length() > 0) {
            qDebug() << "querying for " << q;
            hits = client.query((const char*)q.toUtf8());
        } else {
            hits.hits.clear();
            hits.error = "";
        }
        oldq = q;
        mutex.lock();
        q = querystring;
        mutex.unlock();
    } while (q != oldq);
    mutex.lock();
    results = hits;
    mutex.unlock();
    emit queryFinished(q);
}
QueryExecuter::QueryExecuter() {
}
QueryExecuter::~QueryExecuter() {
    // TODO add a timeout
}
ClientInterface::Hits
QueryExecuter::getResults() {
    mutex.lock();
    ClientInterface::Hits r = results;
    mutex.unlock();
    return r;
}
void
QueryExecuter::query(const QString& q) {
    mutex.lock();
    querystring = q;
    mutex.unlock();
    if (!isRunning()) start();
}
