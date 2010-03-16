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
#ifndef STRIGIASYNCCLIENT
#define STRIGIASYNCCLIENT

#include <strigi/strigiconfig.h>
#include "strigitypes.h"
#include <QtCore/QMutex>

/**
 * Wrapper class for accessing the Strigi search client.
 **/
class STRIGI_QTDBUSCLIENT_EXPORT StrigiAsyncClient : public QObject {
Q_OBJECT
public:
    enum RequestType { Status, Count, Query, Histogram };
    class Request {
    public:
        QString query;
        QString fieldname;
        QString labeltype;
        quint32 max; // -1 means: countHits
        quint32 offset;
        RequestType type;
    };
private:
    bool activeRequest;
    Request lastRequest;
    QList<Request> queue;
    QMutex queuelock;

    void appendRequest(const Request& r);
    void sendNextRequest();
    void sendNextRequest(const Request& r);
private Q_SLOTS:
    void handleStatus(const QDBusMessage&);
    void handleCount(const QDBusMessage&);
    void handleGet(const QDBusMessage&);
    void handleHistogram(const QDBusMessage&);
    void handleError(const QDBusError&, const QDBusMessage&);

public:
    StrigiAsyncClient();
    ~StrigiAsyncClient();
public Q_SLOTS:
    void updateStatus();
    void addCountQuery(const QString& query);
    void addGetQuery(const QString& query, int max, int offset);
    void addGetHistogramRequest(const QString& query, const QString&
       fieldname, const QString& labeltype);
    void clearRequests(RequestType type);

Q_SIGNALS:
    void statusUpdated(const QMap<QString,QString>& status);
    void countedQuery(const QString& query, int count);
    void gotHits(const QString& query, int offset,
        const QList<StrigiHit>& hits);
    void gotHistogram(const QString& query, const QString& fieldname,
        const QList<StringUIntPair>& h);
};

#endif
