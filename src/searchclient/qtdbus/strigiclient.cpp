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
    qDBusRegisterMetaType<QList<BoolStringPair> >();
    qDBusRegisterMetaType<BoolStringPair>();
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
QString
StrigiClient::setIndexedDirectories(const QStringList& d) {
    return p->strigi.setIndexedDirectories(d);
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
QList<QPair<bool,QString> >
StrigiClient::getFilters() const {
    QDBusReply<QList<QPair<bool,QString> > > r = p->strigi.getFilters();
    if (r.isValid()) {
        return r;
    }
    QList<QPair<bool,QString> > f;
    return f;
}
void
StrigiClient::setFilters(const QList<QPair<bool,QString> >& filters) {
    p->strigi.setFilters(filters);
}
QString
StrigiClient::startIndexing() {
    return p->strigi.startIndexing();
}
QString
StrigiClient::stopIndexing() {
    return p->strigi.stopIndexing();
}
QString
StrigiClient::stopDaemon() {
    return p->strigi.stopDaemon();
}
QStringList
StrigiClient::getIndexedFiles() {
    return p->strigi.getIndexedFiles();
}
void
StrigiClient::indexFile(const QString &path, qulonglong mtime,
        const QByteArray &content) {
    p->strigi.indexFile(path, mtime, content);
}
