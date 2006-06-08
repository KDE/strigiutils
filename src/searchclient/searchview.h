#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include <QtGui/QWidget>
#include "qt4kittenclient.h"

class QTextBrowser;
class QUrl;

class SearchView : public QWidget {
Q_OBJECT
private:
    QTextBrowser* view;
    QString query;
    Qt4KittenClient& kitten;
private slots:
    void openItem(const QUrl& url);
public:
    SearchView(Qt4KittenClient& k);
    void setHTML(const QString&html);
public slots:
    void handleHits(const QString&, const ClientInterface::Hits&);
    void setQuery(const QString&);
};

#endif
