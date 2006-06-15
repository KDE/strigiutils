#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include <QtGui/QWidget>
#include "qt4strigiclient.h"

class QTextBrowser;
class QUrl;

class SearchView : public QWidget {
Q_OBJECT
private:
    QTextBrowser* view;
    QString query;
    Qt4StrigiClient& strigi;

    QString iconHTML(const QString& mimetype);
private slots:
    void openItem(const QUrl& url);
public:
    SearchView(Qt4StrigiClient& k);
    void setHTML(const QString&html);
public slots:
    void handleHits(const QString&, const ClientInterface::Hits&);
    void setQuery(const QString&);
};

#endif
