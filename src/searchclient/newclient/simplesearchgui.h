#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H
/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Carsten Niehaus <cniehaus@kde.or>
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
#include <QtGui/QMainWindow>
#include "strigiclient.h"
#include "strigiasyncclient.h"

#include "ui_searchdialog.h"
#include "ui_histogramwidget.h"

class QDockWidget;

class SimpleSearchGui : public QMainWindow 
{
    Q_OBJECT
    private:
        Ui::SearchDialog ui;

        QDockWidget * m_histogramWidget;

        bool starting;
        bool indexing;
        bool running;
        StrigiAsyncClient asyncstrigi;
        StrigiClient strigi;

        void startDaemon();
        void setDirectories();
        void updateDirectories();
        void createActions();
        
    private Q_SLOTS:
        void updateStatus();
        void updateStatus(const QMap<QString, QString>& s);
        void toggleDaemon();
        void toggleIndexing();
        void toggleHistogram();
        void query(const QString&);
        void addDirectory();
        void removeDirectory();
        void editFilters();
        void editListIndexedFiles();
        void refresh();
    
    public:
        explicit SimpleSearchGui(QWidget * parent = 0, Qt::WFlags flags = 0);

    signals:
        void queryChanged( QString );
};

#endif // SIMPLESEARCHGUI_H
