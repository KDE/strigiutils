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
#include "strigihtmlgui.h"
#include "socketclient.h"
#include "indexreader.h"
#include "query.h"
#include "queryparser.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace Strigi;

class StrigiHtmlGui::Private {
private:
    HtmlHelper* h;
    string highlightTerms(const string& t, const Query& q) const;
    void printSearchResult(ostream& out,
        const Strigi::IndexedDocument& doc, const Query& q) const;
public:
    SocketClient strigi;

    Private(HtmlHelper* h);
    void printSearchResults(ostream& out, const ClientInterface::Hits&,
        const string& query) const;
};

StrigiHtmlGui::StrigiHtmlGui(HtmlHelper* h) : helper(h) {
    p = new Private(helper);
}
StrigiHtmlGui::~StrigiHtmlGui() {
    delete p;
}
void
StrigiHtmlGui::printHtmlHeader(ostream& out) {
    // FIXME: extra spaces added at the beginning as a wordaround because
    // KIO discards several first chars for unknown reason
    out << "                           <?xml version='1.0' encoding='utf-8'?>\n"
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><meta http-equiv='Content-Type' "
        "content='text/html; charset=utf-8' />"
        "<link rel='stylesheet' type='text/css' href='"
        << helper->getCssUrl()
        << "'/>"
        "<title>Strigi Desktop Search</title>"
        "</head><body>";
}
void
StrigiHtmlGui::printHeader(ostream& out, const string& path,
        const map<string, string> &params) {
    printHtmlHeader(out);
    out << "<div class='header'>";
    printMenu(out, path, params);
    out << "<div class='title'>Strigi Desktop Search</div>";
    out << "</div><div class='box'>";
}
void
StrigiHtmlGui::printFooter(ostream& out, const string& path,
        const map<string, string> &params) {
    out << "</div>";
    out << "</body></html>";
}
void
StrigiHtmlGui::printHelp(ostream& out, const string& path,
        const map<string, string> &params) {
    out << "For help see the <a href='http://strigi.sf.net'>Strigi Wiki</a>";
}
void
StrigiHtmlGui::printAbout(ostream& out, const string& path,
        const map<string, string> &params) {
    out << "Strigi v"STRIGI_VERSION_STRING"<br/>";
    out << "For more details see <a href='http://strigi.sf.net/'>"
                "the Strigi Website</a>.";
}
void
StrigiHtmlGui::printConfig(ostream& out, const string& path,
        const map<string, string> &params) {
    printIndexedDirs(out, path, params);
}
bool
exists(const char* file) {
    struct stat s;
    if (stat(file, &s)) {
        return false;
    }
    return S_ISREG(s.st_mode);
}
void
startDaemon() {
    char* const daemon = "strigidaemon";
    // find the executable
    // get the PATH environment
    const char* path = getenv("PATH");
    const char* end = strchr(path, ':');
    string p;
    while (end) {
        p.assign(path, end);
        p.append("/");
        p.append(daemon);
        path = end+1;
        end = strchr(path, ':');
        if (exists(p.c_str())) {
            if (fork()) {
                char * const args[] = {daemon, "clucene", 0};
                execvp(p.c_str(), args);
            }
            break;
        }
    }

}
void
StrigiHtmlGui::printStatus(ostream& out, const string& path,
        const map<string, string> &params) {
    map<string, string> status;
    if (path == "status/start") {
        status = p->strigi.getStatus();
        if (status.size() == 0) {
            startDaemon();
            int n = 0;
            while (n < 5 && status.size() == 0) {
                sleep(1);
                status = p->strigi.getStatus();
                n++;
            }
        }
    } else if (path == "status/stop") {
        p->strigi.stopDaemon();
    } else if (path == "status/stopindexing") {
        p->strigi.stopIndexing();
        status = p->strigi.getStatus();
    } else if (path == "status/startindexing") {
        p->strigi.startIndexing();
        status = p->strigi.getStatus();
    } else {
        status = p->strigi.getStatus();
    }
    if (status.size() == 0) {
        out << "<p><a href='/status/start'>Start daemon</a></p>";
    } else {
        map<string, string>::const_iterator i;
        out << "<table>";
        for (i = status.begin(); i != status.end(); ++i) {
            out << "<tr><td>" << i->first << "</td><td>" << i->second
                << "</td><tr>";
        }
        out << "</table>";
        out << "<p><a href='/status/stop'>Stop daemon</a></p>";
        if (status["Status"] == "indexing") {
            out << "<p><a href='/status/stopindexing'>Stop indexing</a></p>";
        } else {
            out << "<p><a href='/status/startindexing'>Start indexing</a></p>";
        }
    }
    // automatically reload the status page
    out << "<script type='text/javascript'>\n"
        "setTimeout('window.location.replace(\"/status\")', 2000);\n"
        "</script>\n";
}
void
StrigiHtmlGui::printSearch(ostream& out, const string& path,
        const map<string, string> &params) {
    string query;
    map<string, string>::const_iterator i = params.find("q");
    if (i != params.end()) query = i->second;
    int max = 10;
    i = params.find("m");
    if (i != params.end()) {
        max = atoi(i->second.c_str());
    }
    if (max <= 0 || max > 1000) max = 10;
    int off = 0;
    i = params.find("o");
    if (i != params.end()) {
        off = atoi(i->second.c_str());
    }
    if (off < 0) {
        off = 0;
    }
    string selectedtab;
    i = params.find("t");
    if (i != params.end()) {
        selectedtab = i->second;
    }

    // print tabs
    int count = 0;
    if (query.length()) {
        count = p->strigi.countHits(query);
    }
    string activetab;
    string activequery = query;
    map<string, int> hitcounts;
    if (count) {
        map<string, string> tabs;
        bool doother = true;
        tabs = readTabQueries();
        if (tabs.size() == 0) {
            tabs["Images"] = "mimeType:image*";
            tabs["Mail"] = "mimeType:message/*";
            tabs["Web"] = "mimeType:text/html";
            tabs["Text"] = "mimeType:text/*";
        }
        map<string, string>::const_iterator j;
        string otherq = query;
        for (j = tabs.begin(); j != tabs.end(); ++j) {
            string q = query;
            if (q != j->second) {
                q += ' ' + j->second;
            }
            int c = p->strigi.countHits(q);
            if (c > 0) {
                hitcounts[j->first] = c;
                doother &= c < count;
                otherq += " -" + j->second;
                if (j->first == selectedtab || activetab.size() == 0) {
                    activetab = j->first;
                    activequery = q;
                }
            }
        }
        doother &= hitcounts.size() > 0;
        int othercount = 0;
        if (doother) {
            othercount = p->strigi.countHits(otherq);
            if (othercount > 0) {
                hitcounts["Other"] = othercount;
                if (selectedtab == "Other") {
                    activetab = selectedtab;
                    activequery = otherq;
                }
            }
        }
    }

    // print gui
    out << "<div class='control' style='text-align:right;' padding='5px'>";
    out << "<form method='get'>";
    out << "<input type='text' name='q' value='" << query << "'/>";
    out << "<input type='hidden' name='m' value='" << max << "'/>";
    out << "<input type='hidden' name='t' value='" << activetab << "'/>";
    out << "<input type='submit' value='search'/>";
    int activecount = count;
    if (hitcounts.size() == 0 && count > 0) {
        out << " Found " << count << " results.";
    } else {
        map<string, int>::const_iterator l;
        for (l = hitcounts.begin(); l != hitcounts.end(); ++l) {
            if (l->first == activetab) {
                out << " <a class='activetab' ";
                activecount = l->second;
            } else {
                out << " <a class='tab' ";
            }
            out << "href='?q=" << query << "&t=" << l->first << "'>"
                << l->first << "&nbsp;(" << l->second << ")" << "</a>";
        }
    }
    out << "</form></div>\n";

    if (activequery.length()) {
        // print results
        const ClientInterface::Hits hits = p->strigi.getHits(activequery,
            max, off);
        if (hits.hits.size()) {
            out << "<div class='hits'>";
            p->printSearchResults(out, hits, activequery);
            out << "</div>";
        }
    }
    if (activecount > max) {
        ostringstream oss;
        oss << ".?q=" << query << "&m=" << max << "&t=" << activetab << "&o=";
        out << "<div class='pager'>";
        int o = 0;
        int n = 1;
        while (o < activecount && n <= 20) {
            out << "<a href='" << oss.str() << o << "'>" << n << "</a> ";
            o += max;
            n++;
        }
        out << "</div>";
    }
}
void
getFields(set<string>& fields, const Query& query) {
    copy(query.fields().begin(), query.fields().end(),
        inserter(fields, fields.begin()));
    for (vector<Query>::const_iterator i = query.subQueries().begin();
            i != query.subQueries().end(); ++i) {
        getFields(fields, *i);
    }
}
void
getTerms(set<string>& terms, const Query& q) {
    if (q.term().string().size() && !q.negate()) {
        terms.insert(q.term().string());
    }
    for (vector<Query>::const_iterator i = q.subQueries().begin();
            i != q.subQueries().end(); ++i) {
        getTerms(terms, *i);
    }
}
string
StrigiHtmlGui::Private::highlightTerms(const string& t, const Query& q) const {
    vector<string> terms;
    set<string> termset;
    getTerms(termset, q);
    copy(termset.begin(), termset.end(), back_inserter(terms));
    string out = h->highlight(t, terms);
    return out;
}
void
StrigiHtmlGui::printMenu(ostream& out, const string& path,
        const map<string, string> &params) {
    out << "<div class='menu'>" << endl;
    out << "<a href='/'>search</a> " << endl;
    out << "<a href='/status'>status</a> " << endl;
    out << "<a href='/config'>preferences</a> " << endl;
    out << "<a href='/help'>help</a> " << endl;
    out << "<a href='/about'>about</a> " << endl;
    out << "</div>" << endl;
}
void
StrigiHtmlGui::printIndexedDirs(ostream& out, const string& path,
        const map<string, string> &params) {
    set<string> dirs = p->strigi.getIndexedDirectories();
    map<string,string>::const_iterator i = params.find("adddir");
    if (i != params.end()) {
        DIR* dir = opendir(i->second.c_str());
        if (dir) {
            dirs.insert(i->second);
            closedir(dir);
            p->strigi.setIndexedDirectories(dirs);
            out << "<p>Directory added. Don't forget to start indexing.</p>";
        }
    }
    i = params.find("deldir");
    if (i != params.end()) {
        uint oldsize = dirs.size();
        dirs.erase(i->second);
        if (dirs.size() != oldsize) {
            p->strigi.setIndexedDirectories(dirs);
        }
    }

    out << "<table>";
    set<string>::const_iterator j;
    for (j = dirs.begin(); j != dirs.end(); ++j) {
        out << "<tr><td><form method='get'>"
            "<input type='hidden' name='deldir' value='" << *j << "'/>"
            "<input type='submit' value='delete directory'/></form></td><td>"
            << *j << "</td></tr>";
    }
    out << "<form><tr><td><input type='submit' value='add directory'/></td>"
        "<td><input name='adddir' type='file'/></td></tr></form>";
    out << "</table>";
}
void
StrigiHtmlGui::printPage(ostream& out, const string& path,
        const map<string, string> &params) {
    printHeader(out, path, params);

    bool running = p->strigi.getStatus().size() > 0;
    if (strncmp(path.c_str(), "help", 4) == 0) {
        printHelp(out, path, params);
    } else if (strncmp(path.c_str(), "about", 5) == 0) {
        printAbout(out, path, params);
    } else if (running && strncmp(path.c_str(), "config", 6) == 0) {
        printConfig(out, path, params);
    } else if (strncmp(path.c_str(), "status", 6) == 0) {
        printStatus(out, path, params);
    } else if (running) {
        printSearch(out, path, params);
    } else {
        printStatus(out, path, params);
    }

    printFooter(out, path, params);
}
StrigiHtmlGui::Private::Private(HtmlHelper* helper) :h(helper) {
    string homedir = getenv("HOME");
    strigi.setSocketName(homedir+"/.strigi/socket");
}
string
toSizeString(int s) {
    ostringstream o;
    if (s > 1024) {
        o << (s+512)/1024 << "k";
    } else {
        o << s << " bytes";
    }
    return o.str();
}
void
StrigiHtmlGui::Private::printSearchResult(ostream& out,
        const Strigi::IndexedDocument& doc, const Query& query) const {
    multimap<string, string>::const_iterator t;
    string link, icon, name, folder;
    int depth = 0;
    t = doc.properties.find("depth");
    if (t != doc.properties.end()) {
        depth = atoi(t->second.c_str());
    }
    link = h->mapLinkUrl(doc.uri, depth);
    icon = h->mapMimetypeIcon(doc.uri, doc.mimetype);
    if (icon.length()) {
        icon = "<div class='iconbox'><img class='icon' src='"+icon;
        icon += "'/></div>\n";
    }
    t = doc.properties.find("title");
    if (t == doc.properties.end()) {
        t = doc.properties.find("subject");
    }
    size_t l = doc.uri.rfind('/');
    if (t != doc.properties.end()) {
        name = t->second.c_str();
    } else if (l != string::npos) {
        name = doc.uri.substr(l+1);
    } else {
        name = doc.uri;
    }
    name = h->escapeString(name);
    if (l != string::npos) {
        folder = doc.uri.substr(0, l);
    }
    out << "<div class='hit'>" << icon << "<h2><a href='" << link << "'>";
    out << name << "</a></h2>";
/*    out << "<br/>score: ";
    out << doc.score << ", mime-type: " << doc.mimetype.c_str() << ", size: ";
    out << doc.size << ", last modified: " << h->formatDate(doc.mtime);*/
    string fragment = h->escapeString(doc.fragment);
    fragment = highlightTerms(fragment, query);
    out << "<div class='fragment'>" << fragment << "</div>";
    string path = h->escapeString(doc.uri);

    out << "<div class='path'>";
    string::size_type p = path.find('/');
    int pp = 0;
    string subpath;
    while (p != string::npos) {
        subpath = path.substr(pp, p-pp+1);
        link = h->mapLinkUrl(path.substr(0, p));
        out << "<a href='" << link << "'>" << subpath << "</a>"
            // the images are meant to allow line break between characters
            //"<img class='softbreak' src=''/><span style=''>/</span>"
            << h->getPathCharacterSeparator();
            //"<img class='softbreak' src=''/>";
        pp = p+1;
        p = path.find('/', pp);
    }
    subpath = path.substr(pp, path.length()-pp+1);
    link = h->mapLinkUrl(doc.uri, depth);
    out << "<a href='" << link << "'>" << subpath << "</a>";

    out << " - "
        << toSizeString(doc.size) << " - "
        << h->mimetypeDescription(doc.mimetype) << "</div>";
    /*out << "<table>";
    map<string, string>::const_iterator j;
    for (j = doc.properties.begin(); j != doc.properties.end(); ++j) {
        out << "<tr><td>" << j->first << ":</td><td>" << j->second.c_str()
            << "</td></tr>";
    }
    out << "</table>";*/
    out << "</div>";
}
void
StrigiHtmlGui::Private::printSearchResults(ostream& out,
        const ClientInterface::Hits& hits, const string& q) const {
    QueryParser parser;
    Query query = parser.buildQuery(q);
    vector<Strigi::IndexedDocument>::const_iterator i;
    for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
        printSearchResult(out, *i, query);
    }
}
/**
 * Read the tab queries from a configuration file.
 * This file is, in the current implementation, located at ~/.strigi/tabqueries.
 * The format is a tab separated table with two columns: the name and the query.
 **/
map<string, string>
StrigiHtmlGui::readTabQueries() const {
    map<string, string> tabs;
    string path = getenv("HOME");
    path += "/.strigi/tabqueries";
    ifstream in;
    in.open(path.c_str());
    string s;
    do {
        getline(in, s);
        if (s.size()) {
            size_t p = s.find('\t');
            if (p != string::npos) {
                string name = s.substr(0, p);
                string value = s.substr(p);
                tabs[name] = value;
            }
        }
    } while (!in.eof() && in.good());
    in.close();
    return tabs;
}
void
StrigiHtmlGui::printSearchResults(std::ostream& out,
        const ClientInterface::Hits& hits, const std::string& query) {
    printHtmlHeader(out);
    p->printSearchResults(out, hits, query);
    out << "</body></html>";
}
