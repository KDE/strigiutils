#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QScrollArea>

class HistogramArea;
class Histogram : public QScrollArea {
private:
    HistogramArea* area;
    int barlength;
    Qt::Orientation orientation;
public:
    Histogram(QWidget* q=0);
    void setData(const QList<QPair<QString,quint32> >& d);
    Qt::Orientation getOrientation() const { return orientation; }
    void setOrientation(Qt::Orientation orientation);
    void setBarLength(int b);
    int getBarLength() const { return barlength; }
    void clear();
};

#endif
