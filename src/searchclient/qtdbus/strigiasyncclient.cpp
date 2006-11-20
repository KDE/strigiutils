#include "strigiasyncclient.h"
#include "strigidbus.h"
#include <QDebug>

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
    appendRequest(r);
}
void
StrigiAsyncClient::addCountQuery(const QString& query) {
    Request r;
    r.query = query;
    r.offset = -1;
    appendRequest(r);
}
void
StrigiAsyncClient::addGetQuery(const QString& query, int max, int offset) {
    Request r;
    r.query = query;
    r.max = max;
    r.offset = offset;
    appendRequest(r);
}
void
StrigiAsyncClient::clearCountQueries() {
    queuelock.lock();
    QList<Request>::iterator i = queue.begin();
    while (i != queue.end()) {
        if (!i->query.isNull() && i->offset == -1) {
            i = queue.erase(i);
        } else {
            i++;
        }
    }
    queuelock.unlock();
}
void
StrigiAsyncClient::clearGetQueries() {
    queuelock.lock();
    QList<Request>::iterator i = queue.begin();
    while (i != queue.end()) {
        if (!i->query.isNull() && i->offset != -1) {
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
    if (r.isValid()) {
        QMap<QString,QString> status = r;
        emit statusUpdated(status);
    }
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
    }
    sendNextRequest();
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
    if (r.query.isNull()) {
        method = QLatin1String("getStatus");
        slot = SLOT(handleStatus(QDBusMessage));
    } else if (r.offset == -1) {
        method = QLatin1String("countHits");
        argumentList << qVariantFromValue(r.query);
        slot = SLOT(handleCount(const QDBusMessage&));
    } else {
        method = QLatin1String("getHits");
        argumentList << qVariantFromValue(r.query) << qVariantFromValue(r.max)
            << qVariantFromValue(r.offset);
        slot = SLOT(handleGet(const QDBusMessage&));
    }
    QDBusMessage msg = QDBusMessage::createMethodCall("vandenoever.strigi",
        "/search", "vandenoever.strigi", method);
    msg.setArguments(argumentList);
    QDBusConnection::sessionBus().callWithCallback(msg, this, slot);
}
