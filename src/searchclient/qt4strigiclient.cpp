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
#include "qt4strigiclient.h"
#include <QDir>
#include <QDebug>

Qt4StrigiClient::Qt4StrigiClient() {
    QString socketpath = QDir::homePath()+"/.strigi/socket";
    socket.setSocketPath((const char*)socketpath.toUtf8());
    mode = Idle;
    poller.setSingleShot(false);
    poller.setInterval(1);
    connect(&poller, SIGNAL(timeout()), this, SLOT(poll()));
}
void
Qt4StrigiClient::poll() {
    if (socket.statusChanged()) {
        poller.stop();
        if (mode == Query) {
            hits = socket.getHits();
            QString query = queryQueue.dequeue();
            emit gotHits(query, hits);
        } else if (mode == CountHits) {
            int count = socket.getHitCount();
            QString query = countQueue.dequeue();
            emit gotHitsCount(query, count);
        }
        mode = Idle;
        if (countQueue.size()) {
            startCountHits();
        } else if (queryQueue.size()) {
            startQuery();
        }
    }
}
void
Qt4StrigiClient::countHits(const QString& query) {
    countQueue.append(query);
    if (mode == Idle) {
        startCountHits();
    }
}
void
Qt4StrigiClient::query(const QString& query) {
    if (query.length()) {
        queryQueue.append(query);
        if (mode == Idle) {
            startQuery();
        }
    }
}
void
Qt4StrigiClient::startCountHits() {
    bool ok = socket.countHits((const char*)countQueue.head().toUtf8());
    if (ok) {
        mode = CountHits;
        poller.start();
    } else {
        // fail in silence
        countQueue.dequeue();
    }
}
void
Qt4StrigiClient::startQuery() {
    bool ok = socket.query((const char*)queryQueue.head().toUtf8(), 10, 0);
    if (ok) {
        mode = Query;
        poller.start();
    } else {
        // fail in silence
        queryQueue.dequeue();
    }
}
