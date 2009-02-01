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
#include "histogram_impl.h"

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
    qDebug() << "QMainWindow ()";
    ui.setupUi( this );
        

    m_histogramWidget = new QDockWidget( this );
    m_histogramWidget->setObjectName( QLatin1String( "searchclient-histogram" ) );
    m_histogramWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_histogramWidget->toggleViewAction()->setChecked( false );

    histogramWidget_Impl * his = new histogramWidget_Impl();
    his->setItems( strigi.getFieldNames() );
    connect( this, SIGNAL(queryChanged(QString)),
            his, SLOT(setQuery(QString)));

    m_histogramWidget->setWidget( his );

    indexing = false;
    running = false;
    starting = true;

    ui.indexeddirs->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
    ui.queryfield->setFocus(Qt::ActiveWindowFocusReason);

    createActions();
    addDockWidget( Qt::LeftDockWidgetArea, m_histogramWidget );
    
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
            his, SLOT(setQuery(const QString&)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), 
            this, SLOT(updateStatus()));
    timer->start(2000);
    asyncstrigi.updateStatus();
}
void
SimpleSearchGui::refresh() 
{
    //qDebug() << "SimpleSearchGui::refresh()";
    emit(ui.queryfield->text());
}

void SimpleSearchGui::createActions() 
{
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

    ui.actionDisplay_Histogram->setStatusTip(tr("Show the Histrogram"));
    connect(ui.actionDisplay_Histogram, SIGNAL(triggered()),
            this, SLOT(toggleHistogram()));
}

void SimpleSearchGui::query(const QString& item) {
    QString query = item.trimmed();
    if (query.length() == 0) {
        ui.tabs->setQuery(QString());
        asyncstrigi.updateStatus();
        ui.mainview->setCurrentIndex(0);
    } else {
        emit queryChanged( item );
        ui.mainview->setCurrentIndex(1);
        ui.tabs->setQuery(query);
    }
    
    emit(ui.queryfield->text());
}

void SimpleSearchGui::updateStatus() {
    //qDebug() << "updateStatus()";
    if (ui.statusview->isVisible()) {
        asyncstrigi.updateStatus();
    }
    
    emit(ui.queryfield->text());
}

void SimpleSearchGui::updateStatus(const QMap<QString, QString>& s) {
    //qDebug() << "updateStatus( const QMap<QString, QString>& s  )";
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
    ui.toggledaemon->setText((running)?tr("stop daemon"):tr("start daemon"));
    ui.toggledaemon->setEnabled(true);

    bool idxng = status["Status"] == "indexing";
    if (idxng != indexing) {
        indexing = idxng;
        ui.toggleindexing->setText((indexing)?tr("stop indexing"):tr("start indexing"));
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
    //qDebug() << "startDaemon() ";

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

void SimpleSearchGui::toggleHistogram()
{
    //Question: Why is the hide() and show() needed? I _think_ 
    //is shouldn't be needed...
    if (m_histogramWidget->toggleViewAction()->isChecked()) {
        m_histogramWidget->toggleViewAction()->setChecked( false );
        m_histogramWidget->hide();
        ui.actionDisplay_Histogram->setText(tr("Show Histrogram"));
    } else{
        m_histogramWidget->toggleViewAction()->setChecked( true );
        m_histogramWidget->show();
        ui.actionDisplay_Histogram->setText(tr("Hide Histrogram"));
    }
}

void SimpleSearchGui::addDirectory() {
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

    for (QList<QListWidgetItem*>::iterator iter = items.begin(); iter != items.end(); ++iter)
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
