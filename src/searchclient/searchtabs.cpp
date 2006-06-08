#include "searchtabs.h"
#include "searchview.h"
#include <QtGui/QTabBar>
#include <QtGui/QVBoxLayout>
#include <QtCore/QDebug>
#include <QtCore/QVariant>

SearchTabs::SearchTabs() {
    tabs = new QTabBar();
    view = new SearchView(kitten);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(tabs);
    layout->addWidget(view);
    setLayout(layout);

    connect(&kitten, SIGNAL(gotHitsCount(const QString&, int)),
        this, SLOT(handleHitsCount(const QString&, int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}
void
SearchTabs::addTab(const QString& name, const QString& query) {
    querynames[query+" "] = name;    
}
void
SearchTabs::setQuery(const QString& query) {
    this->query = query;
    view->setEnabled(false);
    view->setHTML("");
    // remove all tabs
    while (tabs->count()) tabs->removeTab(0);
    // run queries for each name
    QMapIterator<QString, QString> i(querynames);
    while (i.hasNext()) {
        i.next();
        QString tabquery = i.key() + query;
        kitten.countHits(tabquery);
    }
}
void
SearchTabs::handleHitsCount(const QString& query, int count) {
    if (count < 1) return;
    int active = tabs->currentIndex();
    QMapIterator<QString, QString> i(querynames);
    while (i.hasNext()) {
        i.next();
        if (query == i.key() + this->query) {
            QString tabname = i.value() + " (" + QString::number(count) + ")";
            int t = tabs->addTab(tabname);
            tabs->setTabData(t, query);
            break;
        }
    }
    if (active != tabs->currentIndex()) {
        tabChanged(tabs->currentIndex());
    }
}
void
SearchTabs::tabChanged(int) {
    QVariant data = tabs->tabData(tabs->currentIndex());
    QString query = data.toString();
    view->setQuery(query);
}
