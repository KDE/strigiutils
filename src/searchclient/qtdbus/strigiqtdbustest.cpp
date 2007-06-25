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
#include "strigiasyncclient.h"
#include <QCoreApplication>
#include <QtCore/QDebug>

void
printStrigiStatus(StrigiClient& strigi) {
    // query strigi
    QMap<QString,QString> r = strigi.getStatus();
    QTextStream out(stdout);
    QMapIterator<QString, QString> i(r);
    while (i.hasNext()) {
        i.next();
        out << i.key() << ": " << i.value() << endl;
    }
}
void
printStrigiQuery(StrigiClient& strigi, const QString& query) {
    // query strigi
    QList<StrigiHit> r = strigi.getHits(query, 10, 0);
    QTextStream out(stdout);
    QList<StrigiHit> h = r;

    // print the paths for the first 10 hits
    foreach (const StrigiHit& sh, h) {
        out << sh.uri << endl;
    }
}
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    // initialize the interface to Strigi
    StrigiClient strigi;

    // get strigis status
    printStrigiStatus(strigi);

    // do a query
    printStrigiQuery(strigi, "kde");

    StrigiAsyncClient asyncstrigi;
    asyncstrigi.updateStatus();
    asyncstrigi.updateStatus();
    asyncstrigi.updateStatus();
    asyncstrigi.updateStatus();
    asyncstrigi.updateStatus();
    asyncstrigi.updateStatus();
    asyncstrigi.addCountQuery("kde");


    return app.exec();
}
