#include "strigitypes.h"

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
