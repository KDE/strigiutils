#include "strigiasyncwidget.h"
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDebug>

StrigiAsyncWidget::StrigiAsyncWidget() {
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    lineedit = new QLineEdit();
    connect(lineedit, SIGNAL(textChanged(const QString&)),
        this, SLOT(newQuery(const QString&)));
    layout->addWidget(lineedit);

    connect(&strigi,
        SIGNAL(gotHits(const QString&, int, const QList<StrigiHit>&)),
        this,
        SLOT(handleHits(const QString&, int, const QList<StrigiHit>&)));

    listview = new QListWidget();
    layout->addWidget(listview);
}
void
StrigiAsyncWidget::newQuery(const QString& q) {
    strigi.clearGetQueries();
    strigi.addGetQuery(q, 20, 0);
    query = q;
}
void
StrigiAsyncWidget::handleHits(const QString& q, int offset,
        const QList<StrigiHit>& hits) {
    if (query != q) return;
    listview->clear();
    foreach (const StrigiHit& sh, hits) {
        listview->addItem(sh.uri);
    }
}
