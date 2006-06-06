#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include "clientinterface.h"
#include <QtCore/QThread>
#include <QtCore/QMutex>

class QueryExecuter : public QThread {
Q_OBJECT
private:
    QString querystring;
    QMutex mutex;
    ClientInterface::Hits results;

    void run();
public:
    QueryExecuter();
    ~QueryExecuter();
    ClientInterface::Hits getResults();
public slots:
    void query(const QString&);
signals:
    void queryFinished(const QString&);
};

#endif
