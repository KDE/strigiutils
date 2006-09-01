/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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

#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <map>
#include <set>
#include <string>

class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;

class DlgPreferences : public QDialog
{
    Q_OBJECT

    public:
        DlgPreferences(bool running, std::set<std::string>* rules, QWidget *parent = 0);
        virtual ~DlgPreferences() {};
        
    private slots:
        void accept();
        void reject();
        void currentOptionChanged ( QListWidgetItem * current, QListWidgetItem * previous );
        
    private:
        void setupFilteringPage();
        void showPage (int optionNum);
        void showPage (QListWidgetItem* option);
        
        QListWidget *optionsList;
        QStackedWidget *stackedView;
        QPushButton *okButton;
        QPushButton *cancelButton;
        bool m_bDaemonRunning;
        std::set<std::string>* m_rules;
        std::map<QListWidgetItem*, QWidget*> m_pages;
};

#endif
