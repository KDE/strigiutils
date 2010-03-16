/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2008 Jos van den Oever <jos@vandenoever.info>
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
#include "strigicontroller.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtCore/QDebug>

using namespace std;

#define i18n tr

class StrigiController::Private {
public:
    enum Status {
        Unknown, Up, Down, Stopping, StartingViaPing, StartingViaCmdLine
    };
private:
    StrigiController * const parent;
public:
    QLabel* status;
    QPushButton* start;
    QPushButton* stop;
    Status strigistatus;

    Private(StrigiController* p) :parent(p) {
        status = new QLabel();
        start = new QPushButton(i18n("Start Strigi"),p);
        stop = new QPushButton(i18n("Stop Strigi"),p);

        QVBoxLayout* l = new QVBoxLayout(p);
        l->addWidget(status);
        l->addWidget(start);
        l->addWidget(stop);
        p->setLayout(l);

        strigistatus = Unknown;
        checkStatus();
    }
    void checkStatus();
};
void
StrigiController::Private::checkStatus() {
    bool up = QDBusConnection::sessionBus().interface()->isServiceRegistered(
            "vandenoever.strigi");
    if (up) {
        start->setEnabled(false);
        status->setText(tr("Strigi is running."));
        if (strigistatus == Stopping) {
            stop->setEnabled(false);
        } else {
            stop->setEnabled(true);
        }
        strigistatus = Up;
    } else {
        stop->setEnabled(false);
        status->setText(tr("Strigi is not running."));
        if (strigistatus == StartingViaPing) {
            strigistatus = StartingViaCmdLine;
            // TODO: startViaCmdLine();
            start->setEnabled(false);
        } else {
            strigistatus = Down;
            start->setEnabled(true);
        }
    }
}
StrigiController::StrigiController(QWidget * parent, Qt::WindowFlags f)
        :QDialog(parent, f), d(new Private(this)) {
    connect(d->start, SIGNAL(pressed()), this, SLOT(slotStartDaemon()));
    connect(d->stop, SIGNAL(pressed()), this, SLOT(slotStopDaemon()));
    connect(QDBusConnection::sessionBus().interface(), SIGNAL(
        serviceOwnerChanged(const QString&, const QString&, const QString&)),
        this, SLOT(slotCheckStatus()));
    connect(QDBusConnection::sessionBus().interface(), SIGNAL(
        callWithCallbackFailed(const QDBusError&, const QDBusMessage&)),
        this, SLOT(slotCheckStatus()));
}
StrigiController::~StrigiController() {
    delete d;
}
void
StrigiController::slotDBusReply(const QDBusMessage&) {
    d->checkStatus();
}
void
StrigiController::slotDBusError(const QDBusError& e,
        const QDBusMessage&) {
    d->checkStatus();
    qDebug() << e;
}
void
StrigiController::slotStartDaemon() {
    d->start->setEnabled(false);
    d->strigistatus = Private::StartingViaPing;
    //sendMessage("org.freedesktop.DBus.Introspectable", "Introspect");//Peer",
//"Ping");
    sendMessage("vandenoever.strigi", "getStatus");
}
void
StrigiController::slotStopDaemon() {
    d->stop->setEnabled(false);
    // stop the strigidaemon asynchronously
    d->strigistatus = Private::Stopping;
    sendMessage("vandenoever.strigi", "stopDaemon");
}
void
StrigiController::sendMessage(const QString& interface, const QString& method) {
    QList<QVariant> argumentList;
    QDBusMessage msg(QDBusMessage::createMethodCall("vandenoever.strigi",
        "/search", interface, method));
    bool sent = QDBusConnection::sessionBus().callWithCallback(msg, this,
        SLOT(slotDBusReply(const QDBusMessage&)),
        SLOT(slotDBusError(const QDBusError&,const QDBusMessage&)), 15);
    if (!sent) {
        slotDBusError(0, msg);
    }
}
void
StrigiController::slotCheckStatus() {
    d->checkStatus();
}

