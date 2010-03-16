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
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QScrollArea>
#include "strigiasyncclient.h"

class HistogramArea;
class Histogram : public QScrollArea {
Q_OBJECT
private:
    HistogramArea* area;
    int barlength;
    Qt::Orientation orientation;
    StrigiAsyncClient asyncstrigi;
    QString activeFieldname;
    QString activeQuery;
    bool histogramIsUptodate;
private Q_SLOTS:
    void getHistogram(const QString& query, const QString& fieldname,
        const QList<StringUIntPair>& h);
protected:
    void showEvent(QShowEvent&);
public:
    Histogram(QWidget* q=0);
    Qt::Orientation getOrientation() const { return orientation; }
    void setOrientation(Qt::Orientation orientation);
    void setBarLength(int b);
    int getBarLength() const { return barlength; }
    void clear();
    void setData(const QList<QPair<QString,quint32> >& d);
public Q_SLOTS:
    void setQuery(const QString& query);
    void setFieldName(const QString& fieldname);
};

#endif
