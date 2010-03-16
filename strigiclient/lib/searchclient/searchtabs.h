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
#ifndef SEARCHTABS_H
#define SEARCHTABS_H

#include <QtGui/QWidget>
#include <QtCore/QMap>
#include "strigiasyncclient.h"

class QTabBar;
class SearchView;
class SearchTabs : public QWidget {
Q_OBJECT
private:
    QTabBar* tabs;
    SearchView* view;
    QMap<QString, int> tabqueries;
    QMap<QString, QString> querynames;
    QString query;
    StrigiAsyncClient strigi;
private Q_SLOTS:
    void handleHitsCount(const QString& query, int);
    void tabChanged(int);
public:
    SearchTabs();
    void addTab(const QString& name, const QString& query);
    SearchView* getSearchView() { return view; }
public Q_SLOTS:
    void setQuery(const QString& slot);
Q_SIGNALS:
    void activeQueryChanged(const QString& query);
};

#endif
