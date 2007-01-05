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
#include "dlgfilters.h"
#include "filterwidget.h"
 
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
 
DlgFilters::DlgFilters(const QList<QPair<bool,QString> >& filters,
            QWidget *parent)
        : QDialog(parent, Qt::Dialog) {
    setWindowTitle(tr("strigiclient - Edit filters"));
    QLabel* explanation = new QLabel(tr("Define filters that determine which files will be included and which will be excluded from the index, e.g. '*.html' (files ending in '.html') or '.svn/' (directories called '.svn').\n"
        "The filters are applied to the filenames. The top filter is applied first. If the first filter that matches is an 'include' filter, the file will be included, otherwise it will be excluded. If no filter matches, the file will be included."
));
    explanation->setWordWrap(true);
    filterwidget = new FilterWidget();
    filterwidget->setFilters(filters);
    QPushButton* ok = new QPushButton(tr("&Ok"));
    ok->setDefault(true);
    QPushButton* cancel = new QPushButton(tr("&Cancel"));
 
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(explanation);
    layout->addWidget(filterwidget);
    QHBoxLayout* hl = new QHBoxLayout();
    layout->addLayout(hl);
    hl->addStretch();
    hl->addWidget(ok);
    hl->addWidget(cancel);
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
}
const QList<QPair<bool,QString> >&
DlgFilters::getFilters() const {
    return filterwidget->getFilters();
}
