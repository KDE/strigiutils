#include "strigiclient.h"
#include "strigidbus.h"

class StrigiClient::Private {
public:
    VandenoeverStrigiInterface strigi;
    Private() :strigi("vandenoever.strigi", "/search",
       QDBusConnection::sessionBus()) {}
};
StrigiClient::StrigiClient() {
    // register the custom types
    qDBusRegisterMetaType<QMap<QString,QString> >();
    qDBusRegisterMetaType<QMultiMap<int,QString> >();
    qDBusRegisterMetaType<QList<StrigiHit> >();
    qDBusRegisterMetaType<StrigiHit>();
    p = new Private();
}
StrigiClient::~StrigiClient() {
    delete p;
}
int
StrigiClient::countHits(const QString& query) const {
    return p->strigi.countHits(query);
}
QList<StrigiHit>
StrigiClient::getHits(const QString &query, int max, int offset) const {
    return p->strigi.getHits(query, max, offset);
}
QStringList
StrigiClient::getIndexedDirectories() const {
    return p->strigi.getIndexedDirectories();
}
QMap<QString,QString>
StrigiClient::getStatus() const {
    QDBusReply<QMap<QString,QString> > r = p->strigi.getStatus();
    if (r.isValid()) {
        return r;
    }
    QMap<QString,QString> m;
    m["Status"] = "unreachable";
    return m;
}
