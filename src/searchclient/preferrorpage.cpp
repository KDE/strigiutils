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

#include "preferrorpage.h"

#include <QLabel>
#include <QVBoxLayout>

ErrorPage::ErrorPage( QWidget* parent)
    : QWidget (parent)
{
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    label = new QLabel(this);
    vboxLayout->addWidget(label);
   
    label->setText("<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body><center><h1 style=\" color:#ff0000\">WARNING</h1><p>This feature requires a running daemon process</p></center></body></html>");
}
