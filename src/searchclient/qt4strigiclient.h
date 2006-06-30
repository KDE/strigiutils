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
#ifndef QT4STRIGICLIENT_H
#define QT4STRIGICLIENT_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include "asyncsocketclient.h"

class Qt4StrigiClient : public QObject {
Q_OBJECT
private:
    enum Mode { Idle, CountHits, Query };
    Mode mode;
    AsyncSocketClient socket;
    QTimer poller;
    QQueue<QString> countQueue;
    QQueue<QString> queryQueue;

    ClientInterface::Hits hits;

    void startCountHits();
    void startQuery();
public:
    Qt4StrigiClient();
private slots:
    void poll();
public slots:
    void countHits(const QString&);
    void query(const QString&);
signals:
    void gotHitsCount(const QString& query, int count);
    void gotHits(const QString&, const ClientInterface::Hits&);
};

#endif
