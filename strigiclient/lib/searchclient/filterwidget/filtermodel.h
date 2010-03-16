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

#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QAbstractListModel>
#include <QPair>

class FilterModel : public QAbstractListModel {
public:
    explicit FilterModel(QObject* parent) : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return (parent.isValid())?0:filters.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const {
        return (parent.isValid())?0:1;
    }
    Qt::ItemFlags flags( const QModelIndex& index) const {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
        if (index.row() < 0 || index.row() >= (int)filters.size()) {
            return QVariant();
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return filters[index.row()].second;
        } else if (role == Qt::DecorationRole) {
            return (filters[index.row()].first)?Qt::green:Qt::red;
        }

        return QVariant();
    }
    QList<QPair<bool,QString> > filters;
    void include(const QModelIndex &index, bool state);
    void addFilter(const QModelIndex& index, bool state);
    void delFilter(const QModelIndex& index);
    void moveUp(const QModelIndex&);
    void moveDown(const QModelIndex&);

    bool setData(const QModelIndex& index, const QVariant& value,
        int role=Qt::EditRole);
};

#endif
