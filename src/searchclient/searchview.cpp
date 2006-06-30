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
QString
highlightText(const QString& text, const QString query) {
    int pre = 5, post = 5, maxlen = 100;
    QString t = text;
    t.replace("<", "&lt;").replace(">", "&gt;");
    QStringList items = query.split(" ", QString::SkipEmptyParts);
    vector<QRegExp> res;
    QString out;
    for (int i=0; i<items.size(); ++i) {
        items[i].replace('*', "\\w*").replace('?',"\\w");
        res.push_back(QRegExp("\\b("+items[i]+")\\b",Qt::CaseInsensitive));
    }
    int pos = 0;
    int lasts = -1;
    int laste = -1;
    while (pos >= 0 && out.length()+laste-lasts < maxlen) {
        int rep = -1;
        int len;
        for (uint i=0; i<res.size(); ++i) {
            int p = t.indexOf(res[i], pos);
            if (p > 0 && (rep == -1 || p < rep)) {
                rep = p;
                len = items[i].length();
            }
        }
        if (rep >= 0) {
            int s = t.lastIndexOf(" ", rep-pre);
            if (s == -1) s = (rep-pre < 0) ?0 : rep-pre;
            int e = t.indexOf(" ", rep+len+post);
            if (e == -1) e = t.length();
            if (lasts == -1) {
                lasts = s;
            } else if (s > laste) {
                if (out.length() == 0 && lasts > 0) out += "... ";
                out += t.mid(lasts, laste - lasts) + " ... ";
                lasts = s;
            }
            laste = e;
            pos = rep+1;
        } else {
            pos = rep;
        }
    }
    if (lasts != -1) {
        if (out.length() == 0 && lasts > 0) out += "... ";
        out += t.mid(lasts, laste - lasts) + " ... ";
    }
    for (uint i = 0; i < res.size(); ++i) {
        out.replace(res[i], "<b>\\1</b>");
    }
    if (out.length() == 0) {
        out = t.left(100);
    }
    return out;
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
                //html += highlightText(str(i->fragment), q);
                html += str(i->fragment);
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
