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

#include "dlglistindexedfiles.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

using namespace std;

DlgListIndexedFiles::DlgListIndexedFiles(const QStringList& files,
        QWidget* parent)
    : QDialog (parent) {
    setWindowTitle(tr("Indexed Files"));

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    QGroupBox* groupBox = new QGroupBox(this);
    groupBox->setTitle(tr("Files Indexed"));
    groupBox->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* vboxLayout1 = new QVBoxLayout(groupBox);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(9);

    listFiles = new QListWidget(groupBox);
    listFiles->addItems(files);
    listFiles->sortItems();
    vboxLayout1->addWidget(listFiles);

    vboxLayout->addWidget(groupBox);

    QHBoxLayout* hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);

    QSpacerItem* spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);

    okButton = new QPushButton(this);
    okButton->setText(tr ("OK"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    hboxLayout->addWidget(okButton);

    vboxLayout->addLayout(hboxLayout);
}

void DlgListIndexedFiles::accept()
{
    QDialog::accept();
}
