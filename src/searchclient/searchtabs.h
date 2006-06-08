#ifndef SEARCHTABS_H
#define SEARCHTABS_H

#include <QtGui/QWidget>
#include <QtCore/QMap>
#include "qt4kittenclient.h"

class QTabBar;
class SearchView;
class SearchTabs : public QWidget {
Q_OBJECT
private:
    QTabBar* tabs;
    SearchView* view;
    QMap<QString, int> tabqueries;
    QMap<QString, QString> querynames;
    QString query;
    Qt4KittenClient kitten;
private slots:
    void handleHitsCount(const QString& query, int);
    void tabChanged(int);
public:
    SearchTabs();
    void addTab(const QString& name, const QString& query);
public slots:
    void setQuery(const QString& slot);
signals:
    void activeQueryChanged(const QString& query);
};

#endif
