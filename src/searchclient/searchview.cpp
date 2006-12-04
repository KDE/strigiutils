/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
//#include "qt4strigiclient.h"
#include "searchview.h"
#include "strigihtmlgui.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

using namespace std;

class SearchViewHtmlHelper : public HtmlHelper {
private:
    QDateTime time;
public:
    SearchViewHtmlHelper() {}
    ~SearchViewHtmlHelper() {}
    std::string mapLinkUrl(const std::string& url, int depth = 0) {
        return url;
    }
    std::string mapMimetypeIcon(const std::string& url,
            const std::string& mimetype);
    std::string encodeString(const std::string& url);
    std::string escapeString(const std::string& url);
    std::string formatDate(time_t date);
    std::string getCssUrl() { return ":/strigi/result.css"; }
    std::string highlight(const std::string& text,
        const std::vector<std::string>& queryterms);
    std::string mimetypeDescription(const std::string& mimetype) const {
        return mimetype;
    }
};
string
SearchViewHtmlHelper::mapMimetypeIcon(const string& url,
            const string& mimetype) {
    // use gnome icons because they map easily
    QString gnome = "/opt/gnome/share/icons/gnome/48x48/mimetypes/gnome-mime-";
    QString icon(mimetype.c_str());
    icon.replace("/", "-");
    icon = gnome + icon + ".png";
    QFileInfo i(icon);
    QString html;
    if (!i.exists()) {
        icon = "";
    }
    return (const char*)icon.toUtf8();
}
string
SearchViewHtmlHelper::encodeString(const string& url) {
    QUrl u = QUrl::fromEncoded(url.c_str());
    return (const char*)u.toString().toUtf8();
}
string
SearchViewHtmlHelper::escapeString(const string& line) {
    return (const char*)Qt::escape(QString::fromUtf8(line.c_str())).toUtf8();
}
string
SearchViewHtmlHelper::formatDate(time_t date) {
    QDateTime time;
    time.setTime_t(date);
    return (const char*)time.toString().toUtf8();
}
string
SearchViewHtmlHelper::highlight(const std::string& text,
        const std::vector<std::string>& terms) {
    int pre = 5, post = 5;
    int maxlen = 200;
    QString t = QString::fromUtf8(text.c_str(), text.length());
    vector<QRegExp> res;
    QString out;
    for (uint i=0; i<terms.size(); ++i) {
        QString term = QString::fromUtf8(terms[i].c_str(), terms[i].length());
        term.replace('*', "\\w*").replace('?',"\\w");
        res.push_back(QRegExp("\\b("+term+")\\b", Qt::CaseInsensitive));
    }
    int pos = 0;
    int lasts = -1;
    int laste = -1;
    QRegExp nonword("\\W");
    while (pos >= 0 && out.length()+laste-lasts < maxlen) {
        int rep = -1;
        int len = 0;
        for (uint i=0; i<res.size(); ++i) {
            int p = t.indexOf(res[i], pos);
            if (p >= 0 && (rep == -1 || p < rep)) {
                rep = p;
                len = terms[i].length();
            }
        }
        if (rep >= 0) {
            int s = (rep-pre > 0) ?rep-pre :0;
            s = t.lastIndexOf(nonword, s);
            if (s == -1) s = (rep-pre < 0) ?0 : rep-pre;
            int e = t.indexOf(nonword, rep+len+post);
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
        out = t.left(maxlen);
    }
    return (const char*)out.toUtf8();
}
SearchView::SearchView() {
    view = new QTextBrowser();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(view);
    setLayout(layout);
    htmlguihelper = new SearchViewHtmlHelper();
    htmlgui = new StrigiHtmlGui(htmlguihelper);

    connect(&asyncstrigi,
        SIGNAL(gotHits(const QString&, int, const QList<StrigiHit>&)),
        this, SLOT(handleHits(const QString&, int, const QList<StrigiHit>&)));
    connect(view, SIGNAL(anchorClicked(const QUrl&)),
        this, SLOT(openItem(const QUrl&)));
}
SearchView::~SearchView() {
    delete htmlgui;
    delete htmlguihelper;
}
QString str(const string&s) {
    return QString::fromUtf8(s.c_str(), s.length());
}
#include <sstream>
ClientInterface::Hits
toID(const QList<StrigiHit>& hits) {
    ClientInterface::Hits id;
    foreach (const StrigiHit&h, hits) {
        jstreams::IndexedDocument i;
        i.uri.assign(h.uri.toUtf8());
        i.score = h.score;
        i.fragment.assign(h.fragment.toUtf8());
        i.mimetype.assign(h.mimetype.toUtf8());
        i.sha1.assign(h.sha1.toUtf8());
        i.size = h.size;
        i.mtime = h.mtime;
        id.hits.push_back(i);
    }
    return id;
}
void
SearchView::handleHits(const QString& q, int offset,
        const QList<StrigiHit>& hits) {
    view->clear();
    setEnabled(true);
    if (q != query) return;
    QUrl cssurl(":/strigi/result.css");
    QVariant css = "";
    view->document()->addResource(QTextDocument::StyleSheetResource,
        cssurl, css);
    if (hits.size() > 0) {
        ostringstream str;
        ClientInterface::Hits shits = toID(hits);
        htmlgui->printSearchResults(str, shits, (const char*)q.toUtf8());
        QString html(QString::fromUtf8(str.str().c_str()));
        view->setHtml(html);
        emit gotHits(q);
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
    asyncstrigi.clearRequests(StrigiAsyncClient::Query);
    asyncstrigi.addGetQuery(query, 10, 0);
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
