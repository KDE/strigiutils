#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>

class Histogram : public QWidget {
private:
    QList<QPair<QString,quint32> > data;
public:
    Histogram(QWidget* q=0);
    void paintEvent(QPaintEvent *);
    void setData(const QList<QPair<QString,quint32> >& d);
};

#endif
