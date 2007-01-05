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
#include "filterwidget.h"
#include <QApplication>
using namespace std;

int
main(int argc, char** argv) {
    QApplication qapp(argc, argv);
    FilterWidget fw;
    fw.show();

    QList<QPair<bool,QString> > f;
    f.append(qMakePair(true, QString("*.html")));
    f.append(qMakePair(false, QString(".svn/")));
    fw.setFilters(f);

    return qapp.exec();
}
