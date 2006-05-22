#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>
#include <vector>

class QueryExecuter : public QThread {
Q_OBJECT
private:
    QString querystring;
    QMutex mutex;
    std::vector<std::string> results;

    void run();
public:
    QueryExecuter();
    ~QueryExecuter();
    std::vector<std::string> getResults();
public slots:
    void query(const QString&);
signals:
    void queryFinished(const QString&);
};

#endif
