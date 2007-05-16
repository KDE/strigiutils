#ifndef DBUSSERIALIZATION_H
#define DBUSSERIALIZATION_H

#include "clientinterface.h"

class DBusMessageWriter;

DBusMessageWriter& operator<<(DBusMessageWriter&,
    const ClientInterface::Hits& s);

#endif
