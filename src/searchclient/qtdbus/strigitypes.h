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
#ifndef STRIGITYPES_H
#define STRIGITYPES_H

#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QVector>
#include <QtCore/QMetaType>
#include <QtDBus/QtDBus>

typedef QVector<QList<QVariant> > VariantListVector;
Q_DECLARE_METATYPE(VariantListVector)
typedef QList<int> IntegerList;
Q_DECLARE_METATYPE(IntegerList)
typedef QMap<QString, QString> StringStringMap;
Q_DECLARE_METATYPE(StringStringMap)
typedef QMultiMap<int, QString> IntegerStringMultiMap;
Q_DECLARE_METATYPE(IntegerStringMultiMap)
typedef QPair<bool, QString> BoolStringPair;
Q_DECLARE_METATYPE(BoolStringPair)
Q_DECLARE_METATYPE(QList<BoolStringPair>)
typedef QPair<QString, quint32> StringUIntPair;
Q_DECLARE_METATYPE(StringUIntPair)
Q_DECLARE_METATYPE(QList<StringUIntPair>)

// sdsssxxa{sas}
struct StrigiHit {
   QString uri;
   double score;
   QString fragment;
   QString mimetype;
   QString sha1;
   qint64 size;
   qint64 mtime;
   QMap<QString, QStringList> properties;
};
Q_DECLARE_METATYPE(StrigiHit)
Q_DECLARE_METATYPE(QList<StrigiHit>)

QDBusArgument& operator<<(QDBusArgument &a, const StrigiHit &hit);
const QDBusArgument& operator>>(const QDBusArgument &a, StrigiHit& hit);
QDBusArgument& operator<<(QDBusArgument &a, const BoolStringPair &pair);
const QDBusArgument& operator>>(const QDBusArgument &a, BoolStringPair& pair);
QDBusArgument& operator<<(QDBusArgument &a, const StringUIntPair &pair);
const QDBusArgument& operator>>(const QDBusArgument &a, StringUIntPair& pair);

#endif
