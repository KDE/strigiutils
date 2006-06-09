#include "searchview.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
using namespace std;

SearchView::SearchView(Qt4KittenClient& k) :kitten(k) {
    view = new QTextBrowser();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(view);
    setLayout(layout);

    connect(&kitten,
        SIGNAL(gotHits(const QString&, const ClientInterface::Hits&)),
        this, SLOT(handleHits(const QString&, const ClientInterface::Hits&)));
    connect(view, SIGNAL(anchorClicked(const QUrl&)),
        this, SLOT(openItem(const QUrl&)));
}
void
SearchView::handleHits(const QString& q, const ClientInterface::Hits& hits) {
    view->clear();
    setEnabled(true);
    if (q != query) return;
    if (hits.hits.size() > 0) {
        QString html;
        if (hits.error.length() == 0) {
            view->setEnabled(true);
            vector<ClientInterface::Hit>::const_iterator i;
            for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
                QString path = i->uri.c_str();
                QString name;
                map<string, string>::const_iterator t
                    = i->properties.find("title");
                if (t != i->properties.end()) {
                    name = t->second.c_str();
                }
                int l = path.lastIndexOf('/');
                html += "<div><a href='"+path+"'>";
                if (l != -1) {
                    if (name.length()) {
                        html += name;
                    } else {
                        html += path.mid(l+1);
                    }
                    path = path.left(l);
                    html += "</a> from folder <a href='"+path+"'>" + path;
                } else {
                    if (name.length()) {
                        html += name;
                    } else {
                        html += path;
                    }
                }
                html += "</a><br/>score: ";
                html += QString::number(i->score) + "<br/><i>";
                html += QString(i->fragment.c_str()).left(100).replace("<", "&lt;");
                html += "</i><br/><table>";
                map<string, string>::const_iterator j;
                for (j = i->properties.begin(); j != i->properties.end(); ++j) {
                    html += "<tr><td>"+QString(j->first.c_str())+"</td><td>"
                        +QString(j->second.c_str())+"</td></tr>";
                }
                html += "</table></div>";
            }
        } else {
            html = "<h2>";html+=hits.error.c_str();html+="</h2>";
        }
        view->setHtml(html);
    } else {
        view->append("no results");
    }
}
void
SearchView::setHTML(const QString& html) {
    view->setHtml(html);
}
void
SearchView::setQuery(const QString& q) {
    query = q;
    kitten.query(query);
}
void
SearchView::openItem(const QUrl& url) {
    // if the file does not exist on the file system remove items of the end
    // until it does
    QString file = url.toString();
    view->setSource(view->source());
    QFileInfo info(file);
    while (!info.exists()) {
        int pos = file.lastIndexOf('/');
        if (pos <= 0) return;
        file = file.left(pos);
        info.setFile(file);
    }
    if (file.endsWith(".tar") || file.endsWith(".tar.bz2")
            || file.endsWith(".tar.gz")) {
        file = "tar:"+url.toString();
    } else if (file.endsWith(".zip") || file.endsWith(".jar")) {
        file = "zip:"+url.toString();
    }
    QStringList args;
    args << "openURL" << file;
    QProcess::execute("kfmclient", args);
}
