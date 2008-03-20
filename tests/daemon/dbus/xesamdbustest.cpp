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
    xesam = new OrgFreedesktopXesamSearchInterface("", "", QDBusConnection::sessionBus());
    qDebug() << "== XesamDBusTest::setUp() ==";
    waitForStatusIdle(1000);
}

void
XesamDBusTest::tearDown() {
    qDebug() << "== XesamDBusTest::tearDown() ==";
    delete xesam;
}

bool
XesamDBusTest::waitForStatusIdle(int milliseconds) {
/*    QTime timer;
    timer.start();
    QDBusReply<QStringList> reply = xesam.GetState();
    if (reply.error().isValid()) {
        return false;
    }
    while (status != "IDLE" && timer.elapsed() < milliseconds) {
        // sleep for 10 milliseconds
        strigi_nanosleep(10000000);
        status = strigiclient.getStatus()["Status"];
    }
    return status == "idling";*/
    return false;
}

void
XesamDBusTest::testStartSession() {
    qDebug() << "== XesamDBusTest::testStartSession() ==";
}

