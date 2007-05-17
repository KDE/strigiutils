#ifndef DBUSSERIALIZATION_H
#define DBUSSERIALIZATION_H

#include "clientinterface.h"

class DBusMessageReader;
class DBusMessageWriter;
class Variant;

DBusMessageWriter& operator<<(DBusMessageWriter&,
    const ClientInterface::Hits& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const Variant& v);
DBusMessageWriter& operator<<(DBusMessageWriter&,
    const std::vector<std::vector<Variant> >& v);
DBusMessageReader& operator>>(DBusMessageReader&, Variant& v);

#endif
