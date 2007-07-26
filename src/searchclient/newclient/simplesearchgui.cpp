/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Carsten Niehaus <cniehaus@kde.org>
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
#include "simplesearchgui.h"
#include "dlgfilters.h"
#include "dlglistindexedfiles.h"
#include "searchtabs.h"
#include "histogram.h"
#include "searchview.h"
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>
#include <QtGui/QDockWidget>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QComboBox>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QDebug>
#include <string>
#include <map>
#include <vector>
using namespace std;

    SimpleSearchGui::SimpleSearchGui (QWidget * parent, Qt::WFlags flags)
: QMainWindow (parent, flags)
{
    qDebug() << "QMainWindow ()";
    ui.setupUi( this );

    indexing = false;
    running = false;
    starting = true;

    ui.indexeddirs->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui.histogram->setOrientation(Qt::Vertical);
    ui.histogram->setFieldName("system.last_modified_time");

    ui.fieldnames->addItems(strigi.getFieldNames());
    ui.fieldnames->setCurrentIndex(ui.fieldnames->findText("system.last_modified_time"));

    ui.tabs->addTab("kde", "kde");
    ui.tabs->addTab("msg", "content.mime_type:message/*");
    ui.tabs->addTab("irc", "system.location:*konversation*log");
    ui.tabs->addTab("mail", "content.mime_type:text/x-mail");
    ui.tabs->addTab("audio", "content.mime_type:audio/*");
    ui.tabs->addTab("other",
            "-kde -system.location:*konversation*log -content.mime_type:message/* -content.mime_type:text/x-mail -content.mime_type:audio/*");
    ui.tabs->addTab("all", "");
    asyncstrigi.updateStatus();
    ui.mainview->setCurrentIndex(0);
    ui.dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    ui.queryfield->setFocus(Qt::ActiveWindowFocusReason);

    createActions();
    
    connect(ui.queryfield, SIGNAL(textChanged(const QString&)),
            this, SLOT(query(const QString&)));
    connect(ui.toggleindexing, SIGNAL(clicked()),
            this, SLOT(toggleIndexing()));
    connect(ui.toggledaemon, SIGNAL(clicked()),
            this, SLOT(toggleDaemon()));
    connect(ui.adddir, SIGNAL(clicked()),
            this, SLOT(addDirectory()));
    connect(ui.removedir, SIGNAL(clicked()),
            this, SLOT(removeDirectory()));
    connect(&asyncstrigi,SIGNAL(statusUpdated(const QMap<QString, QString>& )),
            this, SLOT(updateStatus(const QMap<QString, QString>& )));
    connect(ui.tabs->getSearchView(), SIGNAL(gotHits(const QString&)),
            ui.histogram, SLOT(setQuery(const QString&)));
    connect(ui.refreshHistogram, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui.fieldnames, SIGNAL(currentIndexChanged(const QString&)),
            ui.histogram, SLOT(setFieldName(const QString&)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    timer->start(2000);
    asyncstrigi.updateStatus();
}
void
SimpleSearchGui::refresh() {
    qDebug() << "refresh()";
    ui.histogram->setQuery(ui.queryfield->text());
}

void
SimpleSearchGui::createActions() {
    ui.actionExit->setShortcut(tr("Ctrl+Q"));
    ui.actionExit->setStatusTip(tr("Quit the program"));
    connect(ui.actionExit, SIGNAL(triggered()), 
            this, SLOT(close()));

    ui.actionEdit_filters->setStatusTip(tr("Edit filename filters"));
    connect(ui.actionEdit_filters, SIGNAL(triggered()), 
            this, SLOT(editFilters()));

    ui.actionList_indexed_files->setStatusTip(tr("Show files indexed by strigi"));
    connect(ui.actionList_indexed_files, SIGNAL(triggered()), 
            this, SLOT(editListIndexedFiles()));
}
void
SimpleSearchGui::query(const QString& item) {
    qDebug() << "query()";
    QString query = item.trimmed();
    if (query.length() == 0) {
        ui.tabs->setQuery(QString());
        asyncstrigi.updateStatus();
        ui.mainview->setCurrentIndex(1);
    } else {
        ui.mainview->setCurrentIndex(0);
        ui.tabs->setQuery(query);
    }
    ui.refreshHistogram->setEnabled(query.length());
}
void SimpleSearchGui::updateStatus() {
    qDebug() << "updateStatus()";
    if (ui.statusview->isVisible()) {
        asyncstrigi.updateStatus();
    }
    if (ui.fieldnames->count() == 0) {
        ui.fieldnames->addItems(strigi.getFieldNames());
        ui.fieldnames->setCurrentIndex(
                ui.fieldnames->findText("system.last_modified_time")
                );
    }
}
void
SimpleSearchGui::updateStatus(const QMap<QString, QString>& s) {
    qDebug() << "updateStatus( const QMap<QString, QString>& s  )";
    //    static bool first = true;
    static bool attemptedstart = false;
    //    if (!first && !ui.statusview->isVisible()) return;
    //    first = false;
    QMap<QString, QString> status (s);
    if (status.size() == 0) {
        running = false;
        ui.actionEdit_filters->setEnabled(false);
        ui.actionList_indexed_files->setEnabled(false);
        status["Status"] = "Daemon is not running";
    } else {
        attemptedstart = true;
        starting = false;
        running = true;
        ui.actionEdit_filters->setEnabled(true);
        ui.actionList_indexed_files->setEnabled(true);
        if (ui.indexeddirs->count() == 0) {
            updateDirectories();
        }
    }
    ui.toggleindexing->setEnabled(running);
    ui.adddir->setEnabled(running);
    ui.removedir->setEnabled(running);
    ui.queryfield->setEnabled(running);
    ui.toggledaemon->setText((running)?"stop daemon":"start daemon");
    ui.toggledaemon->setEnabled(true);

    bool idxng = status["Status"] == "indexing";
    if (idxng != indexing) {
        indexing = idxng;
        ui.toggleindexing->setText((indexing)?"stop indexing":"start indexing");
    }

    QMap<QString,QString>::const_iterator i;
    QString text;
    for (i = status.begin(); i != status.end(); ++i) {
        text += i.key();
        text += ":\t";
        text += i.value();
        text += '\n';
    }
    ui.statusview->setText(text);
}
void
SimpleSearchGui::startDaemon() {
    qDebug() << "startDaemon() ";

    ui.toggledaemon->setEnabled(false);
    starting = true;
    // try to start the daemon
    QFileInfo exe = QCoreApplication::applicationDirPath()
        + "/../../daemon/strigidaemon";
    QStringList args;
    if (exe.exists()) {
        // start not installed version
        QProcess::startDetached(exe.absoluteFilePath(), args);
    } else {
        exe = QCoreApplication::applicationDirPath()+"/strigidaemon";
        if (exe.exists()) {
            QProcess::startDetached(exe.absoluteFilePath(), args);
        } else {
            // start installed version
            QProcess::startDetached("strigidaemon");
        }
    }
}
void
SimpleSearchGui::toggleDaemon() {
    if (running) {
        strigi.stopDaemon();
        ui.indexeddirs->clear();
    } else {
        startDaemon();
    }
}
void
SimpleSearchGui::toggleIndexing() {
    if (indexing) {
        strigi.stopIndexing();
    } else {
        strigi.startIndexing();
    }
}
void
SimpleSearchGui::addDirectory() {
    // open file dialog
    QString dir = QFileDialog::getExistingDirectory (this);
    if (dir.size() <= 0) return;
    for (int i=0; i<ui.indexeddirs->count(); ++i) {
        QString text = ui.indexeddirs->item(i)->text();
        if (dir.startsWith(text)) {
            return;
        }
        if (text.startsWith(dir)) {
            ui.indexeddirs->takeItem(i);
            i = 0;
        }
    }
    ui.indexeddirs->addItem(dir);
    setDirectories();
}
void
SimpleSearchGui::removeDirectory() {
    QList<QListWidgetItem*> items = ui.indexeddirs->selectedItems();

    if (items.size() == 0)
        return;

    for (QList<QListWidgetItem*>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        int row = ui.indexeddirs->row (*iter);
        ui.indexeddirs->takeItem (row);
    }
    setDirectories();
}
void
SimpleSearchGui::setDirectories() {
    QStringList s;
    for (int i=0; i<ui.indexeddirs->count(); ++i) {
        QString text = ui.indexeddirs->item(i)->text();
        s.append(text);
    }
    strigi.setIndexedDirectories(s);
    updateDirectories();
}
void
SimpleSearchGui::updateDirectories() {
    ui.indexeddirs->clear();
    ui.indexeddirs->addItems(strigi.getIndexedDirectories());
}
void
SimpleSearchGui::editFilters() {
    QList<QPair<bool,QString> > filters;
    filters = strigi.getFilters();

    DlgFilters dlg(filters);
    if (dlg.exec() && filters != dlg.getFilters()) {
        strigi.setFilters(dlg.getFilters());
    }
}
void
SimpleSearchGui::editListIndexedFiles() {
    QStringList files = strigi.getIndexedFiles();
    DlgListIndexedFiles dlg(files);
    dlg.exec();
}
