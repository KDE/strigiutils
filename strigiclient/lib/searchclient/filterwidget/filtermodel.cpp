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
#include "filtermodel.h"
#include <QtCore/QDebug>
using namespace std;

void
FilterModel::addFilter(const QModelIndex& index, bool state) {
    
    int r = 1;

    if ( index.isValid() ) {
        r = index.row();
    } else if ( filters.size() > 0 ) {
        r = filters.size();
    }

    beginInsertRows(QModelIndex(),r-1, r);
    filters.insert(filters.begin()+r, qMakePair(state,
        tr("<double-click to edit>")));
    endInsertRows();
}
void
FilterModel::delFilter(const QModelIndex& index) {
    beginRemoveRows(QModelIndex(), filters.size()-1, filters.size());
    filters.erase(filters.begin() + index.row());
    endRemoveRows();
}
void
FilterModel::include(const QModelIndex& index, bool state) {
    if (index.isValid()) {
        filters[index.row()].first = state;
        emit dataChanged(index, index);
    }
}
void
FilterModel::moveUp(const QModelIndex& index) {
    int r = index.row();
    if (index.isValid() && r != 0) {
        QPair<bool,QString> filter = filters[r];
        filters[r] = filters[r-1];
        filters[r-1] = filter;
        emit dataChanged(index.sibling(r-1, 0), index);
    }
}
void
FilterModel::moveDown(const QModelIndex& index) {
    int r = index.row();
    if (index.isValid() && r < (int)filters.size() - 1) {
        QPair<bool,QString> filter = filters[r+1];
        filters[r+1] = filters[r];
        filters[r] = filter;
        emit dataChanged(index, index.sibling(r+1, 0));
    }
}
bool
FilterModel::setData(const QModelIndex& index, const QVariant& value,
        int role) {
    if (index.isValid()) {
        int r = index.row();
        QString s = (const char*)value.toString().toUtf8();
        filters[r].second = s;
    }
    return index.isValid();
}
