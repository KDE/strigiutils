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

#include "dlgpreferences.h"

#include "preferrorpage.h"
#include "preffilterpage.h"

#include <QHBoxLayout>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

using namespace std;

DlgPreferences::DlgPreferences(bool running, set<string>* rules, QWidget *parent)
    : QDialog(parent, Qt::Dialog),
      m_bDaemonRunning (running),
      m_rules (rules)
{
    setWindowTitle ("strigiclient - Edit preferences");
    
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    QHBoxLayout* hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    optionsList = new QListWidget(this);

    hboxLayout->addWidget(optionsList);

    stackedView = new QStackedWidget(this);
    hboxLayout->addWidget(stackedView);

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
    cancelButton->setText(tr("Cancel"));
    hboxLayout1->addWidget(cancelButton);

    vboxLayout->addLayout(hboxLayout1);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    
    setupFilteringPage();
    
    optionsList->sortItems();
    optionsList->resize(optionsList->minimumSizeHint());
    optionsList->setCurrentRow(0);
    showPage (0);
}

void DlgPreferences::currentOptionChanged ( QListWidgetItem * current, QListWidgetItem * previous )
{
    if ((current == 0) || (current == previous))
        return;
    
    showPage (current);
}

void DlgPreferences::showPage (int optionNum)
{
    QListWidgetItem* option = optionsList->item(optionNum);
    
    showPage (option);
}

void DlgPreferences::showPage (QListWidgetItem* option)
{
    stackedView->setCurrentWidget(m_pages[option]);
}

void DlgPreferences::setupFilteringPage()
{
    QListWidgetItem* item = new QListWidgetItem ("Filtering Rules");
    optionsList->addItem (item);
    
    QWidget* widget;
    
    if (m_bDaemonRunning)
        widget = new FilteringRulesPage(m_rules);
    else
        widget = new ErrorPage();
    
    stackedView->addWidget(widget);
    m_pages.insert (make_pair(item, widget));
}

void DlgPreferences::accept()
{
    QDialog::accept();
}

void DlgPreferences::reject()
{
    QDialog::reject();
}
