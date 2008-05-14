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

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>

class QListView;
class FilterModel;
class QModelIndex;
class QPushButton;
class FilterWidget : public QWidget {
Q_OBJECT
private:
    QListView* listview;
    FilterModel* model;
    QPushButton* up;
    QPushButton* down;
    QPushButton* del;
    QPushButton* add;
    QPushButton* incl;

    void updateButtons();
private Q_SLOTS:
    void rowChanged(const QModelIndex&,const QModelIndex&);
    void include(bool state);
    void addFilter();
    void delFilter();
    void moveUp();
    void moveDown();
public:
    FilterWidget(QWidget* parent=0);
    void setFilters(const QList<QPair<bool,QString> >& f);
    const QList<QPair<bool,QString> >& getFilters() const;
};

#endif
