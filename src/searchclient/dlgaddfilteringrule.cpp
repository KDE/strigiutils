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

#include "filters.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

DlgAddFilteringRule::DlgAddFilteringRule(QString* rule, int* type,
        QWidget* parent) : QDialog(parent, Qt::Dialog),
      m_rule (rule),
      m_type (type),
      m_item (NULL)
{
    setupUi();
}

DlgAddFilteringRule::DlgAddFilteringRule(QListWidgetItem* item, QWidget *parent)
    : QDialog(parent, Qt::Dialog),
      m_rule (NULL),
      m_type (NULL),
      m_item (item)
{
    setupUi();

    switch (m_item->type())
    {
        case PathFilter::RTTI:
            pathClicked();
            break;
        case PatternFilter::RTTI:
            patternClicked();
            btnBrowse->hide();
            break;
    }

    lineEdit->setText(item->text());
    rbtnPath->hide();
    rbtnPattern->hide();

    setWindowTitle ("strigiclient - Edit filtering rule");
    label->setText(tr("Edit a filtering rule of type:"));
}

void DlgAddFilteringRule::setupUi ()
{
    setWindowTitle ("strigiclient - Add new filtering rule");

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    label = new QLabel(this);
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

    labelExplanation = new QLabel(this);
    labelExplanation->setText(tr("<p><b>Pattern filtering rule:</b> all files and directories matching the pattern above wont be indexed</p><br><p><b><i>Example:</i></b> pattern <i>*log</i> will prevent <i>foo.log</i> and <i>prolog.pdf</i> from being indexed, but also directory <i>/home/foo/log/</i> and all its contents</p>"));
    labelExplanation->setScaledContents (true);
    labelExplanation->setWordWrap (true);
    vboxLayout->addWidget(labelExplanation);

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
    labelExplanation->setText(tr("<p><b>Path filtering rule:</b> all the contents of the above path wont be indexed</p><br><p><b><i>Example:</i></b> path filter <i>/home/foo/bar/</i> will prevent directory <i>/home/foo/bar/</i> and all its contents from being indexed</p>"));
}

void DlgAddFilteringRule::patternClicked()
{
    labelType->setText(tr("Pattern: "));
    btnBrowse->setEnabled(false);
    labelExplanation->setText(tr("<p><b>Pattern filtering rule:</b> all files and directories matching the pattern above wont be indexed</p><br><p><b><i>Example:</i></b> pattern <i>*log</i> will prevent <i>foo.log</i> and <i>prolog.pdf</i> from being indexed, but also directory <i>/home/foo/log/</i> and all its contents</p>"));
}

void DlgAddFilteringRule::accept()
{
    if ((lineEdit->text()).isEmpty())
    {
        QMessageBox::critical ( 0, QString("error"),
            QString("you haven't specified a filtering rule!"));

        return;
    }

    if (m_item != NULL) {
        m_item->setText (lineEdit->text());
    } else if ((m_type != NULL) && (m_rule != NULL)) {
        if (rbtnPath->isChecked()) {
            *m_type = PathFilter::RTTI;
        } else if (rbtnPattern->isChecked()) {
            *m_type = PatternFilter::RTTI;
        }

        *m_rule = lineEdit->text();
    }
    else
        printf ("DlgAddFilteringRule::accept() : NULL values!\n");

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
