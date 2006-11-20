#ifndef STRIGIASYNCCLIENT
#define STRIGIASYNCCLIENT
#include "strigitypes.h"
#include <QMutex>

/**
 * Wrapper class for accessing the Strigi search client.
 **/
class StrigiAsyncClient : public QObject {
Q_OBJECT
private:
    class Request {
    public:
        QString query; // "" means: getStatus
        int max; // -1 means: countHits
        int offset;
    };
    bool activeRequest;
    Request lastRequest;
    QList<Request> queue;
    QMutex queuelock;

    void appendRequest(const Request& r);
    void sendNextRequest();
    void sendNextRequest(const Request& r);
private slots:
    void handleStatus(const QDBusMessage&);
    void handleCount(const QDBusMessage&);
    void handleGet(const QDBusMessage&);

public:
    StrigiAsyncClient();
    ~StrigiAsyncClient();
public slots:
    void updateStatus();
    void addCountQuery(const QString& query);
    void addGetQuery(const QString& query, int max, int offset);
    void clearCountQueries();
    void clearGetQueries();

signals:
    void statusUpdated(const QMap<QString,QString>& status);
    void countedQuery(const QString& query, int count);
    void gotHits(const QString& query, int offset,
        const QList<StrigiHit>& hits);
};

#endif
