#ifndef STRIGICLIENT
#define STRIGICLIENT
#include "strigitypes.h"

/**
 * Wrapper class for accessing the Strigi search client.
 **/
class StrigiClient {
private:
    class Private;
    Private* p;
public:
    StrigiClient();
    ~StrigiClient();
    int countHits(const QString& query) const;
    QList<StrigiHit> getHits(const QString &query, int max, int offset) const;
    QStringList getIndexedDirectories() const;
    QString setIndexedDirectories(const QStringList& d);
    QMap<QString,QString> getStatus() const;
    QList<QPair<bool,QString> > getFilters() const;
    void setFilters(const QList<QPair<bool,QString> >& rules);
    QString startIndexing();
    QString stopIndexing();
    QString stopDaemon();
    QStringList getIndexedFiles();
    void indexFile(const QString &path, qulonglong mtime,
        const QByteArray &content);
    QList<QPair<QString,quint32> > getHistogram(const QString &query,
        const QString &field);
    QStringList getFieldNames();
/* Functions that are in the DBus interface but are not exposed over this class:
    QStringList getBackEnds();
    bool isActive();*/
};

#endif
