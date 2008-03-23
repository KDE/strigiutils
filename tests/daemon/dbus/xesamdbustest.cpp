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

#include "xesamdbustest.h"

#include "xesamdbus.h"
#include <algorithm>
#include <fstream>
#include <set>

#include <unistd.h>
#include "config.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtTest/QTest>

/**
   This test tests the dbus capabilities of strigidaemon.
 **/

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( XesamDBusTest );

void
XesamDBusTest::setUp() {
    qDebug() << "== XesamDBusTest::setUp() ==";
    xesam = new OrgFreedesktopXesamSearchInterface("org.freedesktop.xesam.searcher",
        "/org/freedesktop/xesam/searcher/main", QDBusConnection::sessionBus());
}

void
XesamDBusTest::tearDown() {
    qDebug() << "== XesamDBusTest::tearDown() ==";
    delete xesam;
}

#define CHECK(REPLY) { const QDBusError error = REPLY.error(); \
   CPPUNIT_ASSERT_MESSAGE((const char*)error.message().toUtf8(), \
   !error.isValid()); }

#define CHECKINVALID(MSG, REPLY) CPPUNIT_ASSERT_MESSAGE(MSG, \
  REPLY.error().isValid());

/**
 * This test opens and closes a session.
 **/
void
XesamDBusTest::testSimpleSession() {
    qDebug() << "== XesamDBusTest::testSimpleSession() ==";
    // try to get the status of the daemon
    CHECK(xesam->GetState());
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // close the session session
    CHECK(xesam->CloseSession(session));
}
void
XesamDBusTest::testSetProperty() {
    qDebug() << "== XesamDBusTest::testSetProperty() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // set a non-existant property
    QDBusReply<QDBusVariant> newValue
       = xesam->SetProperty(session, "nonexistant", QDBusVariant("whatever"));
    CHECKINVALID("Setting this property should fail.", newValue);
    // set a valid property with an invalid value
    newValue
       = xesam->SetProperty(session, "search.live", QDBusVariant("whatever"));
    CHECKINVALID("Setting this property should fail.", newValue);
    // set a valid property with a valid value
    newValue
       = xesam->SetProperty(session, "search.live", QDBusVariant(false));
    CHECK(newValue);

    // close the session session
    CHECK(xesam->CloseSession(session));
}
