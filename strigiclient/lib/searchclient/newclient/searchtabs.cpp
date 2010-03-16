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
#include "searchtabs.h"
#include "searchview.h"
#include <QtGui/QTabBar>
#include <QtGui/QVBoxLayout>
#include <QtCore/QDebug>
#include <QtCore/QVariant>

SearchTabs::SearchTabs(QWidget *  parent) : QWidget(parent)
{
    tabs = new QTabBar();
    tabs->setDrawBase(false);
    view = new SearchView();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(tabs);
    layout->addWidget(view);
    setLayout(layout);

    connect(&strigi, SIGNAL(countedQuery(const QString&, int)),
        this, SLOT(handleHitsCount(const QString&, int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}
void
SearchTabs::addTab(const QString& name, const QString& query) {
    querynames[query+' '] = name;
}
void
SearchTabs::setQuery(const QString& query) {
    this->query = query;
    view->setEnabled(false);
    view->setHTML("");
    // remove all tabs
    while (tabs->count()) tabs->removeTab(0);
    // run queries for each name
    strigi.clearRequests(StrigiAsyncClient::Count);
    QMapIterator<QString, QString> i(querynames);
    while (i.hasNext()) {
        i.next();
        QString tabquery = i.key() + query;
        strigi.addCountQuery(tabquery);
    }
}
void
SearchTabs::handleHitsCount(const QString& query, int count) {
    if (count < 1) return;
    int active = tabs->currentIndex();
    QMapIterator<QString, QString> i(querynames);
    while (i.hasNext()) {
        i.next();
        if (query == i.key() + this->query) {
            QString tabname = i.value() + " (" + QString::number(count) + ')';
            int t = tabs->addTab(tabname);
            tabs->setTabData(t, query);
            break;
        }
    }
    if (active != tabs->currentIndex()) {
        tabChanged(tabs->currentIndex());
    }
}
void
SearchTabs::tabChanged(int) {
    QVariant data = tabs->tabData(tabs->currentIndex());
    QString query = data.toString();
    view->setQuery(query);
}
