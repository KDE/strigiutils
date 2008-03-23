#ifndef XESAMLISTENER_H
#define XESAMLISTENER_H

#include <QtCore/QStringList>
#include <QtCore/QEventLoop>

class OrgFreedesktopXesamSearchInterface;

class XesamListener : public QObject {
Q_OBJECT
private:
    QEventLoop eventloop;
    QStringList finishedSearches;
public:
    XesamListener(OrgFreedesktopXesamSearchInterface*);
    bool waitForSearchToFinish(const QString& searchid,
        int millisecondtimeout);
private slots:
    void slotHitsAdded(const QString &search, uint count);
    void slotHitsModified(const QString &search, const QList<uint> &hit_ids);
    void slotHitsRemoved(const QString &search, const QList<uint> &hit_ids);
    void slotSearchDone(const QString &search);
    void slotStateChanged(const QStringList &state_info);
};

#endif
