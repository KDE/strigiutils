#include "qt4kittenclient.h"
#include <QDir>
#include <QDebug>

Qt4KittenClient::Qt4KittenClient() {
    QString socketpath = QDir::homePath()+"/.kitten/socket";
    socket.setSocketPath((const char*)socketpath.toUtf8());
    mode = Idle;
    poller.setSingleShot(false);
    poller.setInterval(1);
    connect(&poller, SIGNAL(timeout()), this, SLOT(poll()));
    qDebug() << "construct";
}
void
Qt4KittenClient::poll() {
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
Qt4KittenClient::countHits(const QString& query) {
    countQueue.append(query);
    if (mode == Idle) {
        startCountHits();
    }
}
void
Qt4KittenClient::query(const QString& query) {
    queryQueue.append(query);
    if (mode == Idle) {
        startQuery();
    }
}
void
Qt4KittenClient::startCountHits() {
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
Qt4KittenClient::startQuery() {
    bool ok = socket.query((const char*)queryQueue.head().toUtf8());
    if (ok) {
        mode = Query;
        poller.start();
    } else {
        // fail in silence
        queryQueue.dequeue();
    }
}
