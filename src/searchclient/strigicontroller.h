#ifndef STRIGICONTROLLER_H
#define STRIGICONTROLLER_H

#include <QtGui/QDialog>

class QDBusMessage;
class QDBusError;
class StrigiController : public QDialog {
Q_OBJECT
private:
    class Private;
    Private * const d;

    void sendMessage(const QString& interface, const QString& method);
private slots:
    void slotDBusReply(const QDBusMessage&);
    void slotDBusError(const QDBusError&,const QDBusMessage&);
    void slotStartDaemon();
    void slotStopDaemon();
    void slotCheckStatus();
public:
    StrigiController(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~StrigiController();
};

#endif
