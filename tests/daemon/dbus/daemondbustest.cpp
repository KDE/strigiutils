/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info> 
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

#include "daemondbustest.h"

#include <algorithm>
#include <fstream>
#include <set>

#include <unistd.h>
#include "config.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QDir>

/**
   This test tests the dbus capabilities of strigidaemon.
   The script starts the daemon with a non-standard configuration file.
   This non-standard configuration file lets the daemon index only one
   directory. It uses ':memory:' as the directory to store the index.
   The ':memory:' directory is a virtual directory that exists only in the
   memory of the daemon.
   The daemon will also be started under a non-standard dbus service name.
   This ensures that the daemon does no interfere with a possible production
   strigidaemon that might be running.
 **/

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DaemonDBusTest );

void
DaemonDBusTest::setUp() {
    qDebug() << "== DaemonDBusTest::setUp() ==";
    waitForStatusIdle(10000);
}

void
DaemonDBusTest::tearDown() {
    qDebug() << "== DaemonDBusTest::tearDown() ==";
}

bool
DaemonDBusTest::waitForStatusIdle(int milliseconds) {
    QTime timer;
    timer.start();
    QString status = strigiclient.getStatus()["Status"];
    while (status != "idling" && timer.elapsed() < milliseconds) {
        // sleep for 10 milliseconds
        strigi_nanosleep(10000000);
        status = strigiclient.getStatus()["Status"];
    }
    qDebug() << "Waited " << timer.elapsed() << " milliseconds for strigidaemon to start.";
    return status == "idling";
}

void
DaemonDBusTest::testProcess() {
    qDebug() << "== DaemonDBusTest::testProcess() ==";
}

void
DaemonDBusTest::testStatusOfEmptyIndex() {
    qDebug() << "== DaemonDBusTest::testStatusOfEmptyIndex() ==";
    // clear out the index
    QStringList sl = strigiclient.getIndexedDirectories();
    strigiclient.setIndexedDirectories(QStringList());
    strigiclient.startIndexing();
    waitForStatusIdle(10000);
    QMap<QString, QString> status = strigiclient.getStatus();
    qDebug() << "Status: '" << status << "'" << endl;
    CPPUNIT_ASSERT_MESSAGE("Daemon status is too short.", status.size() > 1);
    bool ok;
    int numIndexed = status["Documents indexed"].toInt(&ok);
    CPPUNIT_ASSERT_MESSAGE("Number of documents indexed cannot be determined.", ok);
    CPPUNIT_ASSERT_MESSAGE("Not exactly 0 documents in the index.", numIndexed == 0);
    
    // reindex the original files
    strigiclient.setIndexedDirectories(sl);
    strigiclient.startIndexing();
}
void
DaemonDBusTest::testStopDaemon() {
    qDebug() << "== DaemonDBusTest::testStopDaemon() ==";
}
void
DaemonDBusTest::testIndexing() {
    qDebug() << "== DaemonDBusTest::testIndexing() ==";
    // count the files in the indexing dir
    int docsfound = 0;
    // check how many files are listed in the data dir
    foreach (const QString&d, strigiclient.getIndexedDirectories()) {
        QDir dir(d);
        docsfound += dir.entryList(QDir::Files | QDir::Dirs
            | QDir::NoDotAndDotDot).size(); 
    }
    for (int i=0; i<5; ++i) {
        // start the indexing
        strigiclient.startIndexing();
        waitForStatusIdle(10000);
        QMap<QString, QString> status = strigiclient.getStatus();
        int ndocs = status.value("Documents indexed").toUInt();
        CPPUNIT_ASSERT_MESSAGE("Not the right amount of documents indexed.",
            ndocs == docsfound);
    }
}
void
DaemonDBusTest::testSimpleQuery() {
    qDebug() << "== DaemonDBusTest::testSimpleQuery() ==";
    int nhits = strigiclient.countHits("hello");
    cerr << "nhits hello: " << nhits << endl;
    CPPUNIT_ASSERT(nhits == 2);
    nhits = strigiclient.countHits("world");
    cerr << "nhits world: " << nhits << endl;
    CPPUNIT_ASSERT(nhits == 1);
    nhits = strigiclient.countHits("hello world");
    cerr << "nhits hello world: " << nhits << endl;
    CPPUNIT_ASSERT(nhits == 1);
    //nhits = strigiclient.countHits("\"hello world\"");
    //cerr << "nhits \"hello world\": " << nhits << endl;
    //CPPUNIT_ASSERT(nhits == 1);
}
