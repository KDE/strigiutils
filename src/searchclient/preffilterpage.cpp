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

#include "preffilterpage.h"

#include "dlgaddfilteringrule.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "filters.h"

using namespace std;

FilteringRulesPage::FilteringRulesPage(multimap<int,string>* rules, QWidget* parent)
    : QWidget (parent),
      m_rules (rules)
{
    QHBoxLayout* hboxLayout = new QHBoxLayout(this);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(9);

    ruleList = new QListWidget(this);

    hboxLayout->addWidget(ruleList);

    QVBoxLayout* vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    QSpacerItem* spacerItem = new QSpacerItem(20, 171, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    btnAddRule = new QPushButton(this);
    btnAddRule->setText(tr("Add Rule"));
    vboxLayout->addWidget(btnAddRule);

    btnEditRule = new QPushButton(this);
    btnEditRule->setText(tr("Edit Rule"));
    btnEditRule->setEnabled(false);
    vboxLayout->addWidget(btnEditRule);

    btnDelRule = new QPushButton(this);
    btnDelRule->setText(tr("Del Rule"));
    btnDelRule->setEnabled(false);
    vboxLayout->addWidget(btnDelRule);

    hboxLayout->addLayout(vboxLayout);
   
   
    ruleList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for (multimap<int,string>::iterator iter = m_rules->begin(); iter != m_rules->end(); iter++)
    {
        QListWidgetItem* item = new QListWidgetItem((iter->second).c_str(), ruleList, iter->first);
        
        switch (iter->first)
        {
            case PathFilter::RTTI:
                item->setToolTip("Path filtering rule");
                break;
            case PatternFilter::RTTI:
                item->setToolTip("Pattern filtering rule");
                break;
        }
    }
   
    connect (ruleList, SIGNAL(itemSelectionChanged ()), this, SLOT (selectionChanged()));
    connect (ruleList, SIGNAL(itemDoubleClicked (QListWidgetItem*)), this, SLOT (doubleClick(QListWidgetItem*)));
    connect (btnAddRule, SIGNAL(clicked()), this, SLOT (addRule()));
    connect (btnEditRule, SIGNAL(clicked()), this, SLOT (editRule()));
    connect (btnDelRule, SIGNAL(clicked()), this, SLOT (delRule()));

   
}

FilteringRulesPage::~ FilteringRulesPage()
{
    //update filtering rules
    m_rules->clear();

    for (int i = 0 ; i < ruleList->count(); i++)
    {
        QListWidgetItem * item = ruleList->item(i);
        m_rules->insert(make_pair(item->type(),(item->text()).toStdString()));
    }
}

void FilteringRulesPage::selectionChanged()
{
    if ((ruleList->selectedItems()).size() > 0)
    {
        btnDelRule->setEnabled(true);
        btnEditRule->setEnabled(true);
    }
    else
    {
        btnDelRule->setEnabled(false);
        btnEditRule->setEnabled(false);
    }
}

void FilteringRulesPage::doubleClick( QListWidgetItem* item)
{
    if (item == 0)
        return;
   
    editRule (item);
}

void FilteringRulesPage::addRule()
{
    QString rule;
    int type;
    
    DlgAddFilteringRule dlg (&rule, &type);
    dlg.exec();
   
    if (!rule.isEmpty())
    {
        QListWidgetItem* item = new QListWidgetItem (rule, ruleList, type);
        
        switch (type)
        {
            case PathFilter::RTTI:
                item->setToolTip("Path filtering rule");
                break;
            case PatternFilter::RTTI:
                item->setToolTip("Pattern filtering rule");
                break;
        }
    }
}

void FilteringRulesPage::delRule()
{
    QList<QListWidgetItem*> items = ruleList->selectedItems();
   
    if (items.size() == 0)
        return;

    for (QList<QListWidgetItem*>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        int row = ruleList->row (*iter);
        ruleList->takeItem (row);
    }
}

void FilteringRulesPage::editRule(QListWidgetItem* item)
{
    if (item == 0)
    {
        QList<QListWidgetItem*> items = ruleList->selectedItems();
   
        if (items.size() != 1)
            return;
       
        item = *(items.begin());
    }
   
    DlgAddFilteringRule dlg (item);
    dlg.exec();
}
