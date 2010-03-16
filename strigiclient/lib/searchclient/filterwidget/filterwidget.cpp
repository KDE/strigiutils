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
#include "filterwidget.h"
#include "filtermodel.h"
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAbstractListModel>
#include <QtCore/QDebug>

FilterWidget::FilterWidget(QWidget* p) :QWidget(p) {

    listview = new QListView();
    listview->setViewMode(QListView::ListMode);
    model = new FilterModel(this);
    listview->setModel(model);
    incl = new QPushButton(tr("include"));
    up = new QPushButton(tr("move up"));
    down = new QPushButton(tr("move down"));
    del = new QPushButton(tr("remove filter"));
    add = new QPushButton(tr("add filter"));
    incl->setCheckable(true);

    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(incl);
    vl->addWidget(up);
    vl->addWidget(down);
    vl->addWidget(add);
    vl->addWidget(del);
    vl->addStretch();

    layout->addLayout(vl);
    layout->addWidget(listview);

    setLayout(layout);

    connect(listview->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
        this, SLOT(rowChanged(const QModelIndex&,const QModelIndex&)));

    connect(add,  SIGNAL(clicked(bool)), this, SLOT(addFilter()));
    connect(del,  SIGNAL(clicked(bool)), this, SLOT(delFilter()));
    connect(incl, SIGNAL(toggled(bool)), this, SLOT(include(bool)));
    connect(up,   SIGNAL(clicked(bool)), this, SLOT(moveUp()));
    connect(down, SIGNAL(clicked(bool)), this, SLOT(moveDown()));

    updateButtons();
}
void
FilterWidget::setFilters(const QList<QPair<bool,QString> >& f) {
    model->filters = f;
}
const QList<QPair<bool,QString> >&
FilterWidget::getFilters() const {
    return model->filters;
}
void
FilterWidget::rowChanged(const QModelIndex& current, const QModelIndex& prev) {
    updateButtons();
}
void
FilterWidget::updateButtons() {
    QModelIndex i = listview->currentIndex();
    bool valid = i.isValid();
    int r = i.row();
    
    up->setEnabled(r > 0);
    incl->setEnabled(valid);
    if (valid) {
        bool c = model->filters[r].first;
        incl->setChecked(c);
        incl->setText((c) ?tr("include") :tr("exclude"));
    }
    down->setEnabled(valid && r < (int)model->filters.size()-1);
    del->setEnabled(valid && model->filters.size());
    if (!valid || model->filters.size() == 1) {
        listview->setCurrentIndex(i.sibling(0,0));
    }
    listview->setFocus(Qt::OtherFocusReason);
}
void
FilterWidget::include(bool state) {
    model->include(listview->currentIndex(), state);
    updateButtons();
}
void
FilterWidget::addFilter() {
    QModelIndex i = listview->currentIndex();
    model->addFilter(listview->currentIndex(), incl->isChecked());
    listview->setCurrentIndex(i.sibling(i.row(), 0));
    updateButtons();
}
void
FilterWidget::delFilter() {
    model->delFilter(listview->currentIndex());
    updateButtons();
}
void
FilterWidget::moveUp() {
    QModelIndex i = listview->currentIndex();
    model->moveUp(i);
    listview->setCurrentIndex(i.sibling(i.row()-1, 0));
    updateButtons();
}
void
FilterWidget::moveDown() {
    QModelIndex i = listview->currentIndex();
    model->moveDown(listview->currentIndex());
    listview->setCurrentIndex(i.sibling(i.row()+1, 0));
    updateButtons();
}
