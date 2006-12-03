#include "histogram.h"
#include <QPainter>
#include <QDebug>

Histogram::Histogram(QWidget* p) :QWidget(p) {
}
void
Histogram::setData(const QList<QPair<QString,quint32> >& d) {
    data = d;
    update();
}
typedef QPair<QString, quint32> StringUIntPair;
void
Histogram::paintEvent(QPaintEvent *) {
    if (data.size() == 0) return;
    int w = width();
    int h = height();
    int m = 5;
    int bw = (w-m*(data.size()-1))/data.size();
    uint max = 0;
    foreach (const StringUIntPair& p, data) {
        if (p.second > max) max = p.second;
    }
    if (max <= 0) return;
    QPainter painter(this);
//    painter.setPen(palette().highlightedText().color());
    qreal offset = 0;
    painter.rotate(-90);
    painter.translate(-h, 0);
    foreach (const StringUIntPair& p, data) {
        qreal bh = p.second*h/(qreal)max;
        painter.fillRect(QRectF(0, offset, bh, bw), palette().highlight());
        painter.drawText(QRectF(m, offset, w, bw), Qt::AlignVCenter,
            p.first+": "+QString::number(p.second));
        offset += bw + m;
    }
}
