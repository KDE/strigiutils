#ifndef STRIGITYPES_H
#define STRIGITYPES_H

#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QMetaType>
#include <QtDBus/QtDBus>

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
