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
#include "strigiasyncclient.h"
#include "strigidbus.h"
#include <QtCore/QDebug>

StrigiAsyncClient::StrigiAsyncClient() {
    // register the custom types
    qDBusRegisterMetaType<QMap<QString,QString> >();
    qDBusRegisterMetaType<QMultiMap<int,QString> >();
    qDBusRegisterMetaType<QList<StrigiHit> >();
    qDBusRegisterMetaType<StrigiHit>();

    activeRequest = false;
}
StrigiAsyncClient::~StrigiAsyncClient() {
}
void
StrigiAsyncClient::updateStatus() {
    Request r;
    r.type = Status;
    appendRequest(r);
}
void
StrigiAsyncClient::addCountQuery(const QString& query) {
    Request r;
    r.type = Count;
    r.query = query;
    appendRequest(r);
}
void
StrigiAsyncClient::addGetQuery(const QString& query, int max, int offset) {
    Request r;
    r.type = Query;
    r.query = query;
    r.max = max;
    r.offset = offset;
    appendRequest(r);
}
void
StrigiAsyncClient::addGetHistogramRequest(const QString& query, const QString&
       fieldname, const QString& labeltype) {
    Request r;
    r.type = Histogram;
    r.query = query;
    r.fieldname = fieldname;
    r.labeltype = labeltype;
    appendRequest(r);
}
void
StrigiAsyncClient::clearRequests(RequestType type) {
    queuelock.lock();
    QList<Request>::iterator i = queue.begin();
    while (i != queue.end()) {
        if (i->type == type) {
            i = queue.erase(i);
        } else {
            i++;
        }
    }
    queuelock.unlock();
}
void
StrigiAsyncClient::handleStatus(const QDBusMessage& msg) {
    QDBusReply<QMap<QString,QString> > r = msg;
    QMap<QString,QString> status;
    if (r.isValid()) {
        status = r;
    }
    emit statusUpdated(status);
    sendNextRequest();
}
void
StrigiAsyncClient::handleCount(const QDBusMessage& msg) {
    QDBusReply<int> r = msg;
    if (r.isValid()) {
        int c = r;
        emit countedQuery(lastRequest.query, c);
    }
    sendNextRequest();
}
void
StrigiAsyncClient::handleGet(const QDBusMessage& msg) {
    QDBusReply<QList<StrigiHit> > r = msg;
    if (r.isValid()) {
        QList<StrigiHit> hits = r;
        emit gotHits(lastRequest.query, lastRequest.offset, hits);
    } else {
        qDebug() << r.error().message();
    }
    sendNextRequest();
}
void
StrigiAsyncClient::handleHistogram(const QDBusMessage& msg) {
    QDBusReply<QList<StringUIntPair> > r = msg;
    if (r.isValid()) {
        QList<StringUIntPair> h = r;
        emit gotHistogram(lastRequest.query, lastRequest.fieldname, h);
    } else {
        qDebug() << r.error().message();
    }
    sendNextRequest();
}
void
StrigiAsyncClient::handleError(const QDBusError& err, const QDBusMessage& msg) {
    qDebug() << err;
    qDebug() << msg;
}
int
RequestCmp(const StrigiAsyncClient::Request& a,
        const StrigiAsyncClient::Request& b) {
    return a.type < b.type;
}
void
StrigiAsyncClient::appendRequest(const Request& r) {
    queuelock.lock();
    queue.append(r);
    if (!activeRequest) {
        activeRequest = true;
        Request r(queue.front());
        queue.pop_front();
        sendNextRequest(r);
    }
    queuelock.unlock();
}
void
StrigiAsyncClient::sendNextRequest() {
    queuelock.lock();
    if (!queue.isEmpty()) {
        activeRequest = true;
        Request r(queue.front());
        queue.pop_front();
        sendNextRequest(r);
    } else {
        activeRequest = false;
    }
    queuelock.unlock();
}
void
StrigiAsyncClient::sendNextRequest(const Request& r) {
    QList<QVariant> argumentList;

    lastRequest = r;

    QString method;
    const char* slot;
    switch(r.type) {
    case Status:
    default:
        method = QLatin1String("getStatus");
        slot = SLOT(handleStatus(QDBusMessage));
        break;
    case Count:
        method = QLatin1String("countHits");
        argumentList << qVariantFromValue(r.query);
        slot = SLOT(handleCount(const QDBusMessage&));
        break;
    case Query:
        method = QLatin1String("getHits");
        argumentList << qVariantFromValue(r.query) << qVariantFromValue(r.max)
            << qVariantFromValue(r.offset);
        slot = SLOT(handleGet(const QDBusMessage&));
        break;
    case Histogram:
        method = QLatin1String("getHistogram");
        argumentList << qVariantFromValue(r.query)
            << qVariantFromValue(r.fieldname)
            << qVariantFromValue(r.labeltype);
        slot = SLOT(handleHistogram(const QDBusMessage&));
    }
    QDBusMessage msg = QDBusMessage::createMethodCall("vandenoever.strigi",
        "/search", "vandenoever.strigi", method);
    msg.setArguments(argumentList);
    QDBusConnection::sessionBus().callWithCallback(msg, this, slot,
        SLOT(handleError(const QDBusError&, const QDBusMessage&)));
}
