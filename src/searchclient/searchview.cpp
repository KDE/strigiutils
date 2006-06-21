#include "searchview.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>
using namespace std;

SearchView::SearchView(Qt4StrigiClient& k) :strigi(k) {
    view = new QTextBrowser();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(view);
    setLayout(layout);

    connect(&strigi,
        SIGNAL(gotHits(const QString&, const ClientInterface::Hits&)),
        this, SLOT(handleHits(const QString&, const ClientInterface::Hits&)));
    connect(view, SIGNAL(anchorClicked(const QUrl&)),
        this, SLOT(openItem(const QUrl&)));
}
QString str(const string&s) {
    return QString::fromUtf8(s.c_str(), s.length());
}
void
SearchView::handleHits(const QString& q, const ClientInterface::Hits& hits) {
    view->clear();
    setEnabled(true);
    if (q != query) return;
    QDateTime time;
    if (hits.hits.size() > 0) {
        QString html;
        if (hits.error.length() == 0) {
            view->setEnabled(true);
            vector<jstreams::IndexedDocument>::const_iterator i;
            for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
                QString path(str(i->uri));
                QString name;
                map<string, string>::const_iterator t
                    = i->properties.find("title");
                if (t != i->properties.end()) {
                    name = str(t->second);
                }
                int l = path.lastIndexOf('/');
                html += "<div>";
                html += iconHTML(str(i->mimetype));
                html += "<a href='"+QUrl::toPercentEncoding(path)+"'>";
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
                html += QString::number(i->score) + ", mime-type: "
                    + QString(str(i->mimetype)) + ", size: "
                    + QString::number(i->size) + ", last modified: ";
                time.setTime_t(i->mtime);
                html += time.toString() + "<br/><i>";
                html += QString(str(i->fragment)).left(100).replace("<", "&lt;");
                html += "</i><br/><table>";
                map<string, string>::const_iterator j;
                for (j = i->properties.begin(); j != i->properties.end(); ++j) {
                    html += "<tr><td>"+str(j->first)+"</td><td>"
                        +str(j->second)+"</td></tr>";
                }
                html += "</table></div>";
            }
        } else {
            html = "<h2>";html+=str(hits.error);html+="</h2>";
        }
        view->setHtml(html);
    } else {
        view->append("no results");
    }
}
QString
SearchView::iconHTML(const QString& mimetype) {
    QString gnome = "/opt/gnome/share/icons/gnome/48x48/mimetypes/gnome-mime-";
    QString icon = mimetype;
    icon.replace("/", "-");
    icon = gnome + icon + ".png";
    QFileInfo i(icon);
    QString html;
    if (i.exists()) {
        html = "<img src='"+icon+"'/>";
    }
    return html;
}
void
SearchView::setHTML(const QString& html) {
    view->setHtml(html);
}
void
SearchView::setQuery(const QString& q) {
    query = q;
    strigi.query(query);
}
void
SearchView::openItem(const QUrl& url) {
    // if the file does not exist on the file system remove items of the end
    // until it does
    QString file = QUrl::fromPercentEncoding(url.toString().toAscii());
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
    args << "exec" << file;
    QProcess::execute("kfmclient", args);
}
