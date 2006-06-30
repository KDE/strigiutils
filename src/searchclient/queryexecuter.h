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
#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include "clientinterface.h"
#include <QtCore/QThread>
#include <QtCore/QMutex>

class QueryExecuter : public QThread {
Q_OBJECT
private:
    QString querystring;
    QMutex mutex;
    ClientInterface::Hits results;

    void run();
public:
    QueryExecuter();
    ~QueryExecuter();
    ClientInterface::Hits getResults();
public slots:
    void query(const QString&);
signals:
    void queryFinished(const QString&);
};

#endif
