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
#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QMainWindow>
#include "strigiclient.h"
#include "strigiasyncclient.h"

class QLineEdit;
class QListWidget;
class QStackedWidget;
class QLabel;
class QPushButton;
class QComboBox;
class QWidget;
class QMenu;
class QAction;
class SearchTabs;
class Histogram;

class SimpleSearchGui : public QMainWindow {
Q_OBJECT
private:
    bool starting;
    bool indexing;
    bool running;
    QStackedWidget* mainview;
    SearchTabs* tabs;
    QLabel* statusview;
    QLineEdit* queryfield;
    QListWidget* indexeddirs;
    QPushButton* adddir;
    QPushButton* removedir;
    QPushButton* toggleindexing;
    QPushButton* toggledaemon;
    QPushButton* refreshHistogram;
    QComboBox* backendsList;
    QComboBox* fieldnames;
    QWidget* centralview;
    QMenu *fileMenu;
    QMenu *editMenu;
    QAction *fileExitAct;
    QAction *editFiltersAct;
    QAction *editListIndexedFilesAct;
    StrigiAsyncClient asyncstrigi;
    StrigiClient strigi;
    Histogram* histogram;

    void startDaemon();
    void setDirectories();
    void updateDirectories();
    void createMenus();
    void createActions();
private Q_SLOTS:
    void updateStatus();
    void updateStatus(const QMap<QString, QString>& s);
    void toggleDaemon();
    void toggleIndexing();
    void query(const QString&);
    void addDirectory();
    void removeDirectory();
    void editFilters();
    void editListIndexedFiles();
    void refresh();
public:
    explicit SimpleSearchGui(QWidget * parent = 0, Qt::WFlags flags = 0);
};

#endif
