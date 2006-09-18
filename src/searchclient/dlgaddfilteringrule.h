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

#ifndef DLGADDFILTERINGRULE_H
#define DLGADDFILTERINGRULE_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QToolButton;

class DlgAddFilteringRule : public QDialog
{
    Q_OBJECT

    public:
        DlgAddFilteringRule(QListWidgetItem* item, QWidget *parent = 0);
        DlgAddFilteringRule(QString* rule, int* type, QWidget *parent = 0);
        virtual ~DlgAddFilteringRule() {};
       
    private slots:
        void patternClicked ();
        void pathClicked ();
        void browseClicked ();
        void accept();
        void reject();
       
    private:
        void setupUi();
        
        QString* m_rule;
        int*     m_type;
        QRadioButton *rbtnPattern;
        QRadioButton *rbtnPath;
        QLabel *label;
        QLabel *labelType;
        QLabel *labelExplanation;
        QLineEdit *lineEdit;
        QListWidgetItem* m_item;
        QToolButton *btnBrowse;
        QPushButton *okButton;
        QPushButton *cancelButton;
};

#endif
