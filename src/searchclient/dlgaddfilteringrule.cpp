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

#include "dlgaddfilteringrule.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QToolButton>
#include <QVBoxLayout>

DlgAddFilteringRule::DlgAddFilteringRule(QString* rule, QWidget *parent)
    : QDialog(parent, Qt::Dialog),
      m_rule (rule)
{
    setWindowTitle ("strigiclient - Add new filtering rule");
    
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    QLabel* label = new QLabel(this);
    label->setText(tr("Add a new filtering rule of type:"));
    vboxLayout->addWidget(label);

    QVBoxLayout* vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    
    rbtnPattern = new QRadioButton(this);
    rbtnPattern->setText(tr("Pattern"));
    rbtnPattern->setChecked(true);
    vboxLayout1->addWidget(rbtnPattern);

    rbtnPath = new QRadioButton(this);
    rbtnPath->setText(tr("Path"));
    vboxLayout1->addWidget(rbtnPath);

    vboxLayout->addLayout(vboxLayout1);

    QHBoxLayout* hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    
    labelType = new QLabel(this);
    labelType->setText(tr("Pattern: "));
    hboxLayout->addWidget(labelType);

    lineEdit = new QLineEdit(this);
    hboxLayout->addWidget(lineEdit);

    btnBrowse = new QToolButton(this);
    btnBrowse->setText(tr("..."));
    btnBrowse->setEnabled(false);
    hboxLayout->addWidget(btnBrowse);

    vboxLayout->addLayout(hboxLayout);

    QHBoxLayout* hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    
    QSpacerItem* spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem);

    okButton = new QPushButton(this);
    okButton->setText(tr("OK"));
    hboxLayout1->addWidget(okButton);

    cancelButton = new QPushButton(this);
    cancelButton->setText (tr("Cancel"));
    hboxLayout1->addWidget(cancelButton);

    vboxLayout->addLayout(hboxLayout1);
    
    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    
    connect (rbtnPattern, SIGNAL (clicked()), this, SLOT (patternClicked()));
    connect (rbtnPath, SIGNAL (clicked()), this, SLOT (pathClicked()));
    connect (btnBrowse, SIGNAL (clicked()), this, SLOT (browseClicked()));
}

void DlgAddFilteringRule::pathClicked()
{
    labelType->setText(tr("Path: "));
    btnBrowse->setEnabled(true);
}

void DlgAddFilteringRule::patternClicked()
{
    labelType->setText(tr("Pattern: "));
    btnBrowse->setEnabled(false);
}

void DlgAddFilteringRule::accept()
{
    *m_rule = lineEdit->text();
    
    QDialog::accept();
}

void DlgAddFilteringRule::reject()
{
    QDialog::reject();
}

void DlgAddFilteringRule::browseClicked()
{
    QString dir = QFileDialog::getExistingDirectory (this);
    
    if (!dir.isEmpty())
        lineEdit->setText (dir); 
}
