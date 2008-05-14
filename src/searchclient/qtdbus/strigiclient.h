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
#ifndef STRIGICLIENT
#define STRIGICLIENT
#include <strigi/strigiconfig.h>
#include "strigitypes.h"

/**
 * Wrapper class for accessing the Strigi search client.
 **/
class STRIGI_QTDBUSCLIENT_EXPORT StrigiClient {
private:
    class Private;
    Private* const p;
public:
    StrigiClient();
    ~StrigiClient();
    int countHits(const QString& query) const;
    QList<StrigiHit> getHits(const QString &query, int max, int offset) const;
    QStringList getIndexedDirectories() const;
    QString setIndexedDirectories(const QStringList& d);
    QMap<QString,QString> getStatus() const;
    QList<QPair<bool,QString> > getFilters() const;
    void setFilters(const QList<QPair<bool,QString> >& rules);
    QString startIndexing();
    QString stopIndexing();
    QString stopDaemon();
    QStringList getIndexedFiles();
    void indexFile(const QString &path, qulonglong mtime,
        const QByteArray &content);
    QList<QPair<QString,quint32> > getHistogram(const QString &query,
        const QString &field, const QString& labeltype);
    QStringList getFieldNames();
/* Functions that are in the DBus interface but are not exposed over this class:
    QStringList getBackEnds();
    bool isActive();*/
};

#endif
