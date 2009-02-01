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
#include <string>
#include <map>
#include <vector>
using namespace std;

SimpleSearchGui::SimpleSearchGui (QWidget * parent, Qt::WFlags flags)
    : QMainWindow (parent, flags)
{
    mainview = new QStackedWidget();

    QWidget* statuswidget = new QWidget();
    QVBoxLayout *statuslayout = new QVBoxLayout;
    statuslayout->setMargin(0);
    statusview = new QLabel();
    statusview->setAlignment(Qt::AlignTop);
    statusview->setMargin(25);
    indexing = false;
    running = false;
    starting = true;
    indexeddirs = new QListWidget();
    indexeddirs->setSelectionMode(QAbstractItemView::ExtendedSelection);
    adddir = new QPushButton(tr("add directory"));
    removedir = new QPushButton(tr("remove directory"));
    toggleindexing = new QPushButton(tr("start indexing"));
    toggledaemon = new QPushButton(tr("stop daemon"));
    statuslayout->addWidget(statusview);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(toggledaemon);
    hlayout->addWidget(toggleindexing);
    statuslayout->addLayout(hlayout);

    histogram = new Histogram();
    histogram->setOrientation(Qt::Vertical);
    fieldnames = new QComboBox();
    fieldnames->addItems(strigi.getFieldNames());
    fieldnames->setCurrentIndex(fieldnames->findText("system.last_modified_time"));
    histogram->setFieldName("system.last_modified_time");
    refreshHistogram = new QPushButton(tr("refresh histogram"));
    refreshHistogram->setEnabled(false);

/*    vector<string> backends = ClientInterface::getBackEnds();
    if (backends.size() > 1) {
        backendsList = new QComboBox();
        for (uint i = 0; i< backends.size(); ++i) {
            QString backend = backends[i].c_str();
            backendsList->insertItem(i, backend);
        }
        statuslayout->addWidget(backendsList);
    } else {*/
        backendsList = 0;
//    }
    statuslayout->addWidget(indexeddirs);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(adddir);
    hlayout->addWidget(removedir);
    statuslayout->addLayout(hlayout);
    statuswidget->setLayout(statuslayout);

    tabs = new SearchTabs();
    tabs->addTab("kde", "kde");
    tabs->addTab("msg", "content.mime_type:message/*");
    tabs->addTab("irc", "system.location:*konversation*log");
    tabs->addTab("mail", "content.mime_type:text/x-mail");
    tabs->addTab("audio", "content.mime_type:audio/*");
    tabs->addTab("other",
        "-kde -system.location:*konversation*log -content.mime_type:message/* -content.mime_type:text/x-mail -content.mime_type:audio/*");
    tabs->addTab("all", "");
    mainview->addWidget(tabs);
    mainview->addWidget(statuswidget);
    asyncstrigi.updateStatus();
    mainview->setCurrentIndex(1);

    queryfield = new QLineEdit();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mainview);
    layout->addWidget(queryfield);

/*  The histogram widget is temporarily disabled
    QVBoxLayout* histlayout = new QVBoxLayout();
    QDockWidget* histwidget = new QDockWidget();
    histwidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    QWidget* histw = new QWidget();
    histw->setLayout(histlayout);
    histwidget->setWidget(histw);
    histlayout->addWidget(fieldnames);
    histlayout->addWidget(histogram);
    histlayout->addWidget(refreshHistogram);
    addDockWidget(Qt::LeftDockWidgetArea, histwidget);*/

    centralview = new QWidget();
    centralview->setLayout(layout);
    setCentralWidget(centralview);

    createActions();
    createMenus();

    connect(queryfield, SIGNAL(textChanged(const QString&)),
        this, SLOT(query(const QString&)));
    connect(toggleindexing, SIGNAL(clicked()),
        this, SLOT(toggleIndexing()));
    connect(toggledaemon, SIGNAL(clicked()),
        this, SLOT(toggleDaemon()));
    connect(adddir, SIGNAL(clicked()),
        this, SLOT(addDirectory()));
    connect(removedir, SIGNAL(clicked()),
        this, SLOT(removeDirectory()));
    queryfield->setFocus(Qt::ActiveWindowFocusReason);

    connect(&asyncstrigi,SIGNAL(statusUpdated(const QMap<QString, QString>& )),
        this, SLOT(updateStatus(const QMap<QString, QString>& )));
//    connect(tabs->getSearchView(), SIGNAL(gotHits(const QString&)),
//        histogram, SLOT(setQuery(const QString&)));
    connect(refreshHistogram, SIGNAL(clicked()),
        this, SLOT(refresh()));
    connect(fieldnames, SIGNAL(currentIndexChanged(const QString&)),
        histogram, SLOT(setFieldName(const QString&)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    timer->start(2000);
    asyncstrigi.updateStatus();
}
void
SimpleSearchGui::refresh() {
    histogram->setQuery(queryfield->text());
}
void
SimpleSearchGui::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileExitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(editFiltersAct);
    editMenu->addAction(editListIndexedFilesAct);
}
void
SimpleSearchGui::createActions() {
    fileExitAct = new QAction(tr("&Exit"), this);
    fileExitAct->setShortcut(tr("Ctrl+Q"));
    fileExitAct->setStatusTip(tr("Quit the program"));
    connect(fileExitAct, SIGNAL(triggered()), this, SLOT(close()));

    editFiltersAct = new QAction(tr("Edit Filters"), this);
    editFiltersAct->setStatusTip(tr("Edit filename filters"));
    connect(editFiltersAct, SIGNAL(triggered()), this, SLOT(editFilters()));

    editListIndexedFilesAct = new QAction(tr("List indexed files"), this);
    editListIndexedFilesAct->setStatusTip(tr("Show files indexed by strigi"));
    connect(editListIndexedFilesAct, SIGNAL(triggered()), this, SLOT(editListIndexedFiles()));
}
void
SimpleSearchGui::query(const QString& item) {
    QString query = item.trimmed();
    if (query.length() == 0) {
        tabs->setQuery(QString());
        asyncstrigi.updateStatus();
        mainview->setCurrentIndex(1);
    } else {
        mainview->setCurrentIndex(0);
        tabs->setQuery(query);
    }
    refreshHistogram->setEnabled(query.length());
}
void
SimpleSearchGui::updateStatus() {
    if (statusview->isVisible()) {
        asyncstrigi.updateStatus();
    }
    if (fieldnames->count() == 0) {
        fieldnames->addItems(strigi.getFieldNames());
        fieldnames->setCurrentIndex(
            fieldnames->findText("system.last_modified_time"));
    }
}
void
SimpleSearchGui::updateStatus(const QMap<QString, QString>& s) {
//    static bool first = true;
    static bool attemptedstart = false;
//    if (!first && !statusview->isVisible()) return;
//    first = false;
    QMap<QString, QString> status (s);
    if (status.size() == 0) {
        running = false;
        editFiltersAct->setEnabled(false);
        editListIndexedFilesAct->setEnabled(false);
        status["Status"] = "Daemon is not running";
    } else {
        attemptedstart = true;
        starting = false;
        running = true;
        editFiltersAct->setEnabled(true);
        editListIndexedFilesAct->setEnabled(true);
        if (indexeddirs->count() == 0) {
            updateDirectories();
        }
    }
    toggleindexing->setEnabled(running);
    adddir->setEnabled(running);
    removedir->setEnabled(running);
    queryfield->setEnabled(running);
    toggledaemon->setText((running)?tr("stop daemon"):tr("start daemon"));
    toggledaemon->setEnabled(true);
    if (backendsList) {
        backendsList->setEnabled(!running);
    }
    bool idxng = status["Status"] == "indexing";
    if (idxng != indexing) {
        indexing = idxng;
        toggleindexing->setText((indexing)?tr("stop indexing"):tr("start indexing"));
    }

    QMap<QString,QString>::const_iterator i;
    QString text;
    for (i = status.constBegin(); i != status.constEnd(); ++i) {
        text += i.key();
        text += ":\t";
        text += i.value();
        text += '\n';
    }
    statusview->setText(text);
}
void
SimpleSearchGui::startDaemon() {
    toggledaemon->setEnabled(false);
    starting = true;
    // try to start the daemon
    QFileInfo exe = QCoreApplication::applicationDirPath()
    + "/../../daemon/strigidaemon";
    QStringList args;
    if (backendsList) {
        args += backendsList->currentText();
    }
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
        indexeddirs->clear();
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
    for (int i=0; i<indexeddirs->count(); ++i) {
        QString text = indexeddirs->item(i)->text();
        if (dir.startsWith(text)) {
            return;
        }
        if (text.startsWith(dir)) {
            indexeddirs->takeItem(i);
            i = 0;
        }
    }
    indexeddirs->addItem(dir);
    setDirectories();
}
void
SimpleSearchGui::removeDirectory() {
    QList<QListWidgetItem*> items = indexeddirs->selectedItems();
    
    if (items.size() == 0)
        return;

    for (QList<QListWidgetItem*>::iterator iter = items.begin(); iter != items.end(); ++iter)
    {
        int row = indexeddirs->row (*iter);
        indexeddirs->takeItem (row);
    }
    setDirectories();
}
void
SimpleSearchGui::setDirectories() {
    QStringList s;
    for (int i=0; i<indexeddirs->count(); ++i) {
        QString text = indexeddirs->item(i)->text();
        s.append(text);
    }
    strigi.setIndexedDirectories(s);
    updateDirectories();
}
void
SimpleSearchGui::updateDirectories() {
    indexeddirs->clear();
    indexeddirs->addItems(strigi.getIndexedDirectories());
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
