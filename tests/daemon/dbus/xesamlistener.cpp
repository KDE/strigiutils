#include "xesamlistener.h"
#include "xesam/xesamdbus.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>

XesamListener::XesamListener(OrgFreedesktopXesamSearchInterface* xesam)
        :eventloop(this) {
    connect(xesam, SIGNAL(HitsAdded(const QString&, uint)),
        this, SLOT(slotHitsAdded(const QString&, uint)));
    connect(xesam, SIGNAL(HitsRemoved(const QString &, const QList<uint> &)),
        this, SLOT(slotHitsRemoved(const QString &, const QList<uint> &)));
    connect(xesam, SIGNAL(SearchDone(const QString&)),
        this, SLOT(slotSearchDone(const QString&)));
    connect(xesam, SIGNAL(StateChanged(const QStringList&)),
        this, SLOT(slotStateChanged(const QStringList&)));
}
bool
XesamListener::waitForSearchToFinish(const QString& searchid,
        int millisecondtimeout) {
    QTime time;
    time.start();
    int left = millisecondtimeout;
    bool finished = finishedSearches.contains(searchid);
    while (left > 0 && !finished) {
        QTimer::singleShot(left, &eventloop, SLOT(quit()));
        eventloop.exec();
        left = millisecondtimeout - time.elapsed();
        finished = finishedSearches.contains(searchid);
    }
    return finished;
}
void
XesamListener::slotHitsAdded(const QString &search, uint count) {
    eventloop.quit();
}
void
XesamListener::slotHitsModified(const QString &search,
        const QList<uint> &hit_ids) {
    eventloop.quit();
}
void
XesamListener::slotHitsRemoved(const QString &search,
        const QList<uint> &hit_ids) {
    eventloop.quit();
}
void
XesamListener::slotSearchDone(const QString &search) {
    finishedSearches.append(search);
    eventloop.quit();
}
void
XesamListener::slotStateChanged(const QStringList &state_info) {
    eventloop.quit();
}
