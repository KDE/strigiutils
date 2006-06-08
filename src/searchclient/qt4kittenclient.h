#ifndef QT4KITTENCLIENT_H
#define QT4KITTENCLIENT_H

#include <QObject>

class Qt4KittenClient : public QObject {
Q_OBJECT
public:
    Qt4KittenClient();
    void query(const QString& query);
signals:
    void statusChanged();
};

#endif
