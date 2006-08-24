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
#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QSplitter>

class QDirModel;
class QTreeView;
class QModelIndex;
class QTextBrowser;

class FileBrowser : public QSplitter {
Q_OBJECT
private:
    QTreeView *view;
    QDirModel *model;
    QTextBrowser *browser;
public:
    FileBrowser();
    ~FileBrowser();
public slots:
    void clicked(const QModelIndex& index);
};

#endif
