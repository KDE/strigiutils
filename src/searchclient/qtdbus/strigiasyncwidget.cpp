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
#include "strigiasyncwidget.h"
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QtCore/QDebug>

StrigiAsyncWidget::StrigiAsyncWidget() {
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    lineedit = new QLineEdit();
    connect(lineedit, SIGNAL(textChanged(const QString&)),
        this, SLOT(newQuery(const QString&)));
    layout->addWidget(lineedit);

    connect(&strigi,
        SIGNAL(gotHits(const QString&, int, const QList<StrigiHit>&)),
        this,
        SLOT(handleHits(const QString&, int, const QList<StrigiHit>&)));

    listview = new QListWidget();
    layout->addWidget(listview);
}
void
StrigiAsyncWidget::newQuery(const QString& q) {
    strigi.clearRequests(StrigiAsyncClient::Query);
    strigi.addGetQuery(q, 20, 0);
    query = q;
}
void
StrigiAsyncWidget::handleHits(const QString& q, int offset,
        const QList<StrigiHit>& hits) {
    if (query != q) return;
    listview->clear();
    foreach (const StrigiHit& sh, hits) {
        listview->addItem(sh.uri);
    }
}
