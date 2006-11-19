#ifndef STRIGIASYNCWIDGET_H
#define STRIGIASYNCWIDGET_H

#include <QWidget>
#include "strigiasyncclient.h"
class QLineEdit;
class QListWidget;
class StrigiAsyncWidget : public QWidget {
Q_OBJECT
private:
    QString query;
    StrigiAsyncClient strigi;
    QLineEdit* lineedit;
    QListWidget* listview;
public:
    StrigiAsyncWidget();
private slots:
    void newQuery(const QString&);
    void handleHits(const QString&, int offset, const QList<StrigiHit>&);
};

#endif
