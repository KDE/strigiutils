#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QScrollArea>
#include "strigiasyncclient.h"

class HistogramArea;
class Histogram : public QScrollArea {
Q_OBJECT
private:
    HistogramArea* area;
    int barlength;
    Qt::Orientation orientation;
    StrigiAsyncClient asyncstrigi;
    QString activeFieldname;
    QString activeQuery;
    bool histogramIsUptodate;
private slots:
    void getHistogram(const QString& query, const QString& fieldname,
        const QList<StringUIntPair>& h);
protected:
    void showEvent(QShowEvent&);
public:
    Histogram(QWidget* q=0);
    Qt::Orientation getOrientation() const { return orientation; }
    void setOrientation(Qt::Orientation orientation);
    void setBarLength(int b);
    int getBarLength() const { return barlength; }
    void clear();
    void setData(const QList<QPair<QString,quint32> >& d);
public slots:
    void setQuery(const QString& query);
    void setFieldName(const QString& fieldname);
};

#endif
