#include "strigitypes.h"
#include <QtCore/QDebug>

QDBusArgument&
operator<<(QDBusArgument &a, const StrigiHit &hit) {
   a.beginStructure();
   a << hit.uri << hit.score << hit.fragment << hit.mimetype << hit.sha1
       << hit.size << hit.mtime << hit.properties;
   a.endStructure();
   return a;
}
const QDBusArgument&
operator>>(const QDBusArgument &a, StrigiHit& hit) {
   a.beginStructure();
   a >> hit.uri >> hit.score >> hit.fragment >> hit.mimetype >> hit.sha1
       >> hit.size >> hit.mtime >> hit.properties;
   a.endStructure();
   return a;
}
QDBusArgument&
operator<<(QDBusArgument &a, const BoolStringPair &pair) {
   a.beginStructure();
   a << pair.first << pair.second;
   a.endStructure();
   return a;
}
const QDBusArgument&
operator>>(const QDBusArgument &a, BoolStringPair& pair) {
   a.beginStructure();
   a >> pair.first >> pair.second;
   a.endStructure();
   return a;
}
QDBusArgument&
operator<<(QDBusArgument &a, const StringUIntPair &pair) {
    a.beginStructure();
    a << pair.first << pair.second;
    a.endStructure();
    return a;
}
const QDBusArgument& operator>>(const QDBusArgument &a, StringUIntPair& pair) {
    a.beginStructure();
    a >> pair.first >> pair.second;
    a.endStructure();
    return a;
}
