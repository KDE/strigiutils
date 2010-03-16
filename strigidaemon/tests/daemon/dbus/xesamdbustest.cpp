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
#include "xesamlistener.h"

#include "xesam/xesamdbus.h"
#include <algorithm>
#include <fstream>
#include <set>

#include <unistd.h>
#include "config.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

/**
   This test tests the dbus capabilities of strigidaemon.
 **/

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( XesamDBusTest );

QString
userQuery(const QString& s) {
    return "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<request xmlns='http://freedesktop.org/standards/xesam/1.0/query'>\n"
        " <userQuery>"+s+"</userQuery>\n"
        "</request>\n";
}

void
XesamDBusTest::setUp() {
    qDebug() << "== XesamDBusTest::setUp() ==";
    qDBusRegisterMetaType<VariantListVector>();
    qDBusRegisterMetaType<IntList>();
    xesam = new OrgFreedesktopXesamSearchInterface("org.freedesktop.xesam.searcher",
        "/org/freedesktop/xesam/searcher/main", QDBusConnection::sessionBus());
    listener = new XesamListener(xesam);
}

void
XesamDBusTest::tearDown() {
    qDebug() << "== XesamDBusTest::tearDown() ==";
    delete listener;
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
/**
 * This test does a simple search.
 **/
void
XesamDBusTest::testSimpleSearch() {
    qDebug() << "== XesamDBusTest::testSimpleSearch() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // check if the server gives on an invalid query
    QDBusReply<QString> search = xesam->NewSearch(session, "invalid xml");
    CHECKINVALID("This search should fail.", search);
    // check that the server gives an error on an invalid session
    search = xesam->NewSearch("nosession", userQuery("hello"));
    CHECKINVALID("This search should fail.", search);
    // check that the server gives back a valid search id
    search = xesam->NewSearch(session, userQuery("hello"));
    CHECK(search);
    // these functions should give an error when they are called before the
    // search is started
    CHECKINVALID("This should not be possible when a search has not yet been "
        "started.", xesam->GetHitCount(search));
    CHECKINVALID("This should not be possible when a search has not yet been "
        "started.", xesam->GetHits(search, 100));

    // check that the search can be started
    CHECK(xesam->StartSearch(search));
    // check that an invalid search cannot be started
    CHECKINVALID("It should not be possible to start and invalid search.",
        xesam->StartSearch("invalid search id"));
    // we should be able to retrieve the number of hits now
    CHECK(xesam->GetHitCount(search));
    // also getting the next hits should be possible
    CHECK(xesam->GetHits(search, 100));

    // check for an error on closing a non-existent search
    CHECKINVALID("Closing an invalid search should not be possible.",
        xesam->CloseSearch("invalid search id"));
    // close the search object
    CHECK(xesam->CloseSearch(search));
    // closing the search object should only work once
    CHECKINVALID("closing a search should only be possible once.",
        xesam->CloseSearch(search));
    // close the session session
    CHECK(xesam->CloseSession(session));
    // closing the session object should only work once
    CHECKINVALID("closing a session should only be possible once.",
        xesam->CloseSession(session));
}
#define CHECKTYPE(NAME, TYPE) { \
   QDBusReply<QDBusVariant> reply = xesam->GetProperty(session, NAME); \
   CHECK(reply); \
   CPPUNIT_ASSERT_MESSAGE(NAME, reply.value().variant().canConvert<TYPE>()); }
void
XesamDBusTest::testGetProperty() {
    qDebug() << "== XesamDBusTest::testGetProperty() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    CHECKTYPE("search.live", bool);
    CHECKTYPE("hit.fields", QStringList);
    CHECKTYPE("hit.fields.extended", QStringList);
    CHECKTYPE("hit.snippet.length", uint32_t);
    CHECKTYPE("sort.primary", QString);
    CHECKTYPE("sort.secondary", QString);
    CHECKTYPE("sort.order", QString);
    CHECKTYPE("vendor.id", QString);
    CHECKTYPE("vendor.version", uint32_t);
    CHECKTYPE("vendor.display", QString);
    CHECKTYPE("vendor.xesam", uint32_t);
    CHECKTYPE("vendor.xesam", uint32_t);
    CHECKTYPE("vendor.ontology.fields", QStringList);
    CHECKTYPE("vendor.ontology.contents", QStringList);
    CHECKTYPE("vendor.ontology.sources", QStringList);
    CHECKTYPE("vendor.extensions", QStringList);
    CHECKTYPE("vendor.extensions", QStringList);
    // this complex type is not easy to check :-(
    // CHECKTYPE("vendor.ontologies", QList<QStringList> );
    QDBusReply<QDBusVariant> ontologies = xesam->GetProperty(session, "vendor.ontologies");
    CHECK(ontologies);
    QDBusArgument arg = qvariant_cast<QDBusArgument>(ontologies.value().variant());
    CPPUNIT_ASSERT(arg.currentSignature() == "aas");
    QList<QStringList> l;
    arg >> l;
    CHECKTYPE("vendor.maxhits", uint32_t);
}
void
XesamDBusTest::testSetProperty() {
    qDebug() << "== XesamDBusTest::testSetProperty() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // set a non-existent property
    QDBusReply<QDBusVariant> newValue
       = xesam->SetProperty(session, "nonexistent", QDBusVariant("whatever"));
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
void
XesamDBusTest::testSimpleSearchSignals() {
    qDebug() << "== XesamDBusTest::testSimpleSearchSignals() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // turn off live search
    QDBusReply<QDBusVariant> livesearch = xesam->SetProperty(session,
        "search.live", QDBusVariant(false));
    CHECK(livesearch);
    // check that the server gives back a valid search id
    QDBusReply<QString> search = xesam->NewSearch(session,
        userQuery("hello"));
    CHECK(search);
    // check that the search can be started
    CHECK(xesam->StartSearch(search));
    // check that the search is finite
    CPPUNIT_ASSERT_MESSAGE("Search did not finish.",
        listener->waitForSearchToFinish(search, 1000));
    // check that the number of hits is available
    QDBusReply<uint> hitcount = xesam->GetHitCount(search);
    CHECK(hitcount);
    // check that this matches the number that was reported via HitsAdded
    CPPUNIT_ASSERT_MESSAGE("Number of hits reported is not consistent.",
        hitcount == listener->getNumberOfReportedHits(search));

    // close the search object
    CHECK(xesam->CloseSearch(search));
    // close the session session
    CHECK(xesam->CloseSession(session));
}
void
XesamDBusTest::testTwoTermSearch() {
    qDebug() << "== XesamDBusTest::testTwoTermSearch() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    // check that the server gives back a valid search id
    QDBusReply<QString> search = xesam->NewSearch(session,
        userQuery("hello world"));
    CHECK(search);
    // check that the search can be started
    CHECK(xesam->StartSearch(search));
    // we should be able to retrieve the number of hits now
    QDBusReply<uint32_t> count = xesam->GetHitCount(search);
    CPPUNIT_ASSERT(count == 1);
    // also getting the next hits should be possible
    QDBusReply<QVector<QList<QVariant> > > hits = xesam->GetHits(search, 100);
    CHECK(hits);
    CPPUNIT_ASSERT(hits.value().size() == 1);
    CPPUNIT_ASSERT(hits.value()[0].size() == 1);
    CPPUNIT_ASSERT(hits.value()[0].first() == QString("testdatadir/test too"));
    // close the search object
    CHECK(xesam->CloseSearch(search));
    // close the session session
    CHECK(xesam->CloseSession(session));
}
void
XesamDBusTest::testHitFields() {
    qDebug() << "== XesamDBusTest::testHitFields() ==";
    // start a new session
    QDBusReply<QString> session = xesam->NewSession();
    CHECK(session);
    CHECK(xesam->SetProperty(session, "hit.fields",
            QDBusVariant(QStringList() << "nie:url" << "nie:contentSize")));
    // check that the server gives back a valid search id
    QDBusReply<QString> search = xesam->NewSearch(session, userQuery("hello"));
    CHECK(search);
    // check that the search can be started
    CHECK(xesam->StartSearch(search));
    // we should be able to retrieve the number of hits now
    QDBusReply<uint32_t> count = xesam->GetHitCount(search);
    CPPUNIT_ASSERT(count == 2);
    // also getting the next hits should be possible
    QDBusReply<QVector<QList<QVariant> > > hits = xesam->GetHits(search, 100);
    CHECK(hits);
    CPPUNIT_ASSERT(hits.value().size() == 2);
    CPPUNIT_ASSERT(hits.value()[0].size() == 2);
    CPPUNIT_ASSERT(hits.value()[0].first() == QString("testdatadir/test"));
    CPPUNIT_ASSERT(hits.value()[1].first() == QString("testdatadir/test too"));
    CPPUNIT_ASSERT(hits.value()[0].last() == 6);
    CPPUNIT_ASSERT(hits.value()[1].last() == 12);
    // close the search object
    CHECK(xesam->CloseSearch(search));
    // close the session session
    CHECK(xesam->CloseSession(session));
}
