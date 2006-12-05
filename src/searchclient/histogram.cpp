#include "histogram.h"
#include <QPainter>
#include <QMouseEvent>
#include <QScrollBar>
#include <QScrollBar>
#include <QScrollBar>
#include <QDebug>

class HistogramArea : public QWidget {
private:
    QList<QPair<QString,quint32> > data;
    Histogram* h;
    int activeEntry;
    int margin;
    int barwidth;
    uint max;

    void paintBar(QPainter&p, int entry, int barheight);
protected:
    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);
public:
    HistogramArea(Histogram* h);
    void setData(const QList<QPair<QString,quint32> >& d);
    void clear();
};

Histogram::Histogram(QWidget* p) :QScrollArea(p) {
    setFrameStyle(QFrame::NoFrame);
    setOrientation(Qt::Horizontal);
    area = new HistogramArea(this);
    setBarLength(100);
    setWidget(area);
    setWidgetResizable(true);
}
void
Histogram::setData(const QList<QPair<QString,quint32> >& d) {
    area->setData(d);
}
void
Histogram::clear() {
    area->clear();
}
void
Histogram::setBarLength(int b) {
    barlength = b;
    setMinimumSize(b + verticalScrollBar()->width(),
        b + horizontalScrollBar()->height());
}
void
Histogram::setOrientation(Qt::Orientation o) {
    orientation = o;
    if (o == Qt::Vertical) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    } else {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}
HistogramArea::HistogramArea(Histogram* h) :QWidget(h) {
    this->h = h;
    margin = 5;
    QFontMetrics f(font());
    barwidth = (int)(1.3*f.height());
    setMouseTracking(true);
    activeEntry = -1;
}
void
HistogramArea::clear() {
    data.clear();
    int w = h->getBarLength();
    setMinimumSize(w, w);
    update();
}
typedef QPair<QString, quint32> StringUIntPair;
void
HistogramArea::setData(const QList<QPair<QString,quint32> >& d) {
    data = d;
    int width = h->getBarLength();
    int height = (int)(d.size()*(barwidth+margin)-margin);
    if (height < 0) height = 0;
    if (h->getOrientation() == Qt::Vertical) {
        setMinimumSize(width, height);
    } else {
        setMinimumSize(height, width);
    }
    max = 0;
    foreach (const StringUIntPair& p, data) {
        if (p.second > max) max = p.second;
    }
    update();
}
void
HistogramArea::mouseDoubleClickEvent(QMouseEvent* event) {
    if (h->getOrientation() == Qt::Vertical) {
        h->setOrientation(Qt::Horizontal);
    } else {
        h->setOrientation(Qt::Vertical);
    }
    setData(data);
}
void
HistogramArea::mouseMoveEvent(QMouseEvent* event) {
    int item;
    if (h->getOrientation() == Qt::Vertical) {
        item = event->y();
    } else {
        item = event->x();
    }
    qreal n = item / (barwidth + margin);
    int pos = item - (int)((int)n) * (barwidth + margin);
    //int pos = item % (int)(barwidth+margin);
    if (pos >= barwidth) {
        item = -1;
    } else {
        item = item / (int)(barwidth + margin);
        if (item >= data.size()) {
            item = -1;
        }
    }
    if (item != activeEntry) {
        int min, max;
        if (item == -1) {
            min = activeEntry;
            max = min;
        } else if (activeEntry == -1) {
            min = item;
            max = min;
        } else {
            min = (item < activeEntry) ?item :activeEntry;
            max = (item < activeEntry) ?activeEntry :item;
        }
        min *= barwidth + margin;
        max = max * (barwidth + margin) + barwidth - min + 1;
        activeEntry = item;
        if (this->h->getOrientation() == Qt::Horizontal) {
            update(min, 0, max, height());
        } else {
            update(0, min, width(), max);
        }
    }
}
void
HistogramArea::leaveEvent(QEvent* event) {
    if (activeEntry != -1) {
        activeEntry = -1;
        update();
    }
}
void
HistogramArea::paintEvent(QPaintEvent* e) {
    if (data.size() == 0 || max == 0) return;
    int w = width();
    int h = height();
    QPainter painter(this);
    int barheight;
    if (this->h->getOrientation() == Qt::Horizontal) {
        painter.rotate(-90);
        painter.translate(-h, 0);
        barheight = h;
    } else {
        barheight = w;
    }
    for (int i = 0; i < data.size(); ++i) {
        paintBar(painter, i, barheight);
    }
/*    foreach (const StringUIntPair& p, data) {
        qreal bh;
        if (activeEntry == i++) {
            bh = barheight;
        } else {
            bh = p.second*barheight/(qreal)max;
        }
        painter.fillRect(QRectF(0, offset, bh, barwidth),
            palette().highlight());
        painter.drawText(QRectF(margin, offset, w, barwidth), Qt::AlignVCenter,
            p.first+": "+QString::number(p.second));
        offset += barwidth + margin;
    }*/
}
void
HistogramArea::paintBar(QPainter&p, int entry, int barheight) {
    qreal bh;
    if (activeEntry == entry) {
        bh = barheight;
    } else {
        bh = data[entry].second*barheight/(qreal)max;
    }
    int offset = entry*(barwidth+margin);
    p.fillRect(QRectF(0, offset, bh, barwidth), palette().highlight());
    p.drawText(QRectF(margin, offset, barheight, barwidth), Qt::AlignVCenter,
        data[entry].first + ":" + QString::number(data[entry].second));
}
