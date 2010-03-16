/* This file is part of Strigi Desktop Search
 *
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
#include "histogram_impl.h"

#include <QDebug>

histogramWidget_Impl::histogramWidget_Impl(QWidget * parent)
    : QWidget( parent )
{
    ui.setupUi( this );

    ui.histogram->setOrientation( Qt::Vertical );

    connect(ui.refreshHistogram, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui.fieldnames, SIGNAL(currentIndexChanged(const QString&)),
            ui.histogram, SLOT(setFieldName(const QString&)));
}

void histogramWidget_Impl::setQuery( QString query )
{
    qDebug() << "histogramWidget_Impl::setQuery: " << query ;

    m_query = query;

    refresh();
}


void histogramWidget_Impl::refresh()
{
    qDebug() << "histogramWidget_Impl::refresh()";

    //I wonder why Jos wanted to updated the fieldnames...
    //FIXME ???
//X     if (ui.fieldnames->count() == 0) 
//X     {
//X         ui.fieldnames->addItems(strigi.getFieldNames());
//X         ui.fieldnames->setCurrentIndex( ui.fieldnames->findText("system.last_modified_time") );
//X     }

    ui.histogram->setQuery(m_query);
}

void histogramWidget_Impl::setItems( const QStringList& items )
{
    qDebug() << "histogramWidget_Impl::setItems()";
    ui.fieldnames->addItems( items );

    //FIXME this is of course a hack
    ui.histogram->setFieldName("system.last_modified_time");
    ui.fieldnames->setCurrentIndex(ui.fieldnames->findText("system.last_modified_time"));
}
