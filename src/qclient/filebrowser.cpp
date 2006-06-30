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
#include "filebrowser.h"
#include <QtGui/QTreeView>
#include <QtGui/QDirModel>
#include <QtGui/QTextBrowser>
#include <QtGui/QHeaderView>
#include <QDebug>

FileBrowser::FileBrowser() : QSplitter(Qt::Vertical) {
    model = new QDirModel();
    view = new QTreeView();
    view->setModel(model);
    QModelIndex index = model->index(QDir::currentPath());
    view->setRootIndex(index);

    browser = new QTextBrowser();

    addWidget(view);
    addWidget(browser);

    connect(view, SIGNAL(clicked(const QModelIndex&)),
        SLOT(clicked(const QModelIndex&)));

    int cols = model->columnCount(index);
    for (int i=0; i<cols; ++i) {
        view->resizeColumnToContents(i);
    }
}
FileBrowser::~FileBrowser() {
//    delete browser;
//    delete view;
//    delete model;
}
void
FileBrowser::clicked(const QModelIndex& index) {
    int cols = model->columnCount(index);
    for (int i=0; i<cols; ++i) {
        view->resizeColumnToContents(i);
    }
    QString s = model->filePath(index);
    browser->setSource(s);
}

