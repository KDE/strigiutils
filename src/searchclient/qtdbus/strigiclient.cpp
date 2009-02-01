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
#include "strigiclient.h"
#include "strigidbus.h"

class StrigiClient::Private {
public:
    VandenoeverStrigiInterface strigi;
    Private() :strigi("vandenoever.strigi", "/search",
       QDBusConnection::sessionBus()) {}
};
StrigiClient::StrigiClient() :p(new Private()) {
    // register the custom types
    qDBusRegisterMetaType<QMap<QString,QString> >();
    qDBusRegisterMetaType<QMultiMap<int,QString> >();
    qDBusRegisterMetaType<QList<StrigiHit> >();
    qDBusRegisterMetaType<StrigiHit>();
    qDBusRegisterMetaType<QList<BoolStringPair> >();
    qDBusRegisterMetaType<BoolStringPair>();
    qDBusRegisterMetaType<StringUIntPair>();
    qDBusRegisterMetaType<QList<StringUIntPair> >();
}
StrigiClient::~StrigiClient() {
    delete p;
}
int
StrigiClient::countHits(const QString& query) const {
    return p->strigi.countHits(query);
}
QList<StrigiHit>
StrigiClient::getHits(const QString &query, int max, int offset) const {
    return p->strigi.getHits(query, max, offset);
}
QStringList
StrigiClient::getIndexedDirectories() const {
    return p->strigi.getIndexedDirectories();
}
QString
StrigiClient::setIndexedDirectories(const QStringList& d) {
    return p->strigi.setIndexedDirectories(d);
}
QStringList
StrigiClient::getFieldNames() {
    return p->strigi.getFieldNames();
}
QMap<QString,QString>
StrigiClient::getStatus() const {
    QDBusReply<QMap<QString,QString> > r = p->strigi.getStatus();
    if (r.isValid()) {
        return r;
    }
    QMap<QString,QString> m;
    m["Status"] = "unreachable";
    return m;
}
QList<QPair<bool,QString> >
StrigiClient::getFilters() const {
    QDBusReply<QList<QPair<bool,QString> > > r = p->strigi.getFilters();
    if (r.isValid()) {
        return r;
    }
    QList<QPair<bool,QString> > f;
    return f;
}
void
StrigiClient::setFilters(const QList<QPair<bool,QString> >& filters) {
    p->strigi.setFilters(filters);
}
QString
StrigiClient::startIndexing() {
    return p->strigi.startIndexing();
}
QString
StrigiClient::stopIndexing() {
    return p->strigi.stopIndexing();
}
QString
StrigiClient::stopDaemon() {
    return p->strigi.stopDaemon();
}
QStringList
StrigiClient::getIndexedFiles() {
    return p->strigi.getIndexedFiles();
}
void
StrigiClient::indexFile(const QString &path, qulonglong mtime,
        const QByteArray &content) {
    p->strigi.indexFile(path, mtime, content);
}
QList<QPair<QString,quint32> >
StrigiClient::getHistogram(const QString &query, const QString &field,
        const QString& labeltype) {
    return p->strigi.getHistogram(query, field, labeltype);
}
