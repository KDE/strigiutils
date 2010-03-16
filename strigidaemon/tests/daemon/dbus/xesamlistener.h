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
#ifndef XESAMLISTENER_H
#define XESAMLISTENER_H

#include <QtCore/QStringList>
#include <QtCore/QEventLoop>
#include <QtCore/QMap>

class OrgFreedesktopXesamSearchInterface;

class XesamListener : public QObject {
Q_OBJECT
private:
    QEventLoop eventloop;
    QStringList finishedSearches;
    QMap<QString, uint> hitsReported;
public:
    XesamListener(OrgFreedesktopXesamSearchInterface*);
    bool waitForSearchToFinish(const QString& searchid,
        int millisecondtimeout);
    uint getNumberOfReportedHits(const QString& searchid) const;
private slots:
    void slotHitsAdded(const QString &search, uint count);
    void slotHitsModified(const QString &search, const QList<uint> &hit_ids);
    void slotHitsRemoved(const QString &search, const QList<uint> &hit_ids);
    void slotSearchDone(const QString &search);
    void slotStateChanged(const QStringList &state_info);
};

#endif
