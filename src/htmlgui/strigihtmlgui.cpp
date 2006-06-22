#include "strigihtmlgui.h"
#include "socketclient.h"
using namespace std;
using namespace jstreams;

class StrigiHtmlGui::Private {
private:
    HtmlHelper* h;
public:
    SocketClient strigi;

    Private(HtmlHelper* h);
    void printSearchResult(std::ostream& out,
        const jstreams::IndexedDocument& doc);
    void printSearchResults(std::ostream& out, const ClientInterface::Hits&);
};

StrigiHtmlGui::StrigiHtmlGui(HtmlHelper* h) : helper(h) {
    p = new Private(helper);
}
StrigiHtmlGui::~StrigiHtmlGui() {
    delete p;
}
void
StrigiHtmlGui::printHeader(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "<html><title>Strigi Desktop Search</title></head><body>";
    out << "<h1>Strigi Desktop Search</h1>";
}
void
StrigiHtmlGui::printFooter(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "</body></html>";
}
void
StrigiHtmlGui::printHelp(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "Help!";
}
void
StrigiHtmlGui::printAbout(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "About";
}
void
StrigiHtmlGui::printConfig(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "configuration";
}
void
StrigiHtmlGui::printStatus(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    out << "status...";
}
void
StrigiHtmlGui::printSearch(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    std::string query;
    map<string, string>::const_iterator i = params.find("q");
    if (i != params.end()) query = i->second;
    int max = 10;
    i = params.find("m");
    if (i != params.end()) {
        max = atoi(i->second.c_str());
    }
    int off = 0;
    i = params.find("o");
    if (i != params.end()) {
        off = atoi(i->second.c_str());
    }

    out << "<form method='get'>";
    out << "<input type='text' name='q' value='" << query << "'/>";
    out << "<input type='hidden' name='o' value='" << off << "'/>";
    out << "<input type='hidden' name='m' value='" << max << "'/>";
    out << "<input type='submit'/></form>";

    const ClientInterface::Hits hits = p->strigi.getHits(query, max, off);
    p->printSearchResults(out, hits);
}
void
StrigiHtmlGui::printPage(ostream& out, const string& path,
        const std::map<std::string, std::string> &params) {
    printHeader(out, path, params);

    if (strncmp(path.c_str(), "help", 4) == 0) {
        printHelp(out, path, params);
    } else if (strncmp(path.c_str(), "about", 5) == 0) {
        printAbout(out, path, params);
    } else if (strncmp(path.c_str(), "config", 6) == 0) {
        printConfig(out, path, params);
    } else if (strncmp(path.c_str(), "status", 6) == 0) {
        printStatus(out, path, params);
    } else {
        printSearch(out, path, params);
    }

    printFooter(out, path, params);
}
StrigiHtmlGui::Private::Private(HtmlHelper* helper) :h(helper) {
    string homedir = getenv("HOME");
    strigi.setSocketName(homedir+"/.strigi/socket");
}
void
StrigiHtmlGui::Private::printSearchResult(std::ostream& out,
        const jstreams::IndexedDocument& doc) {
    string link, icon, name, folder;
    link = h->mapLinkUrl(doc.uri);
    icon = "<img style='float:left;' class='icon' src='";
    icon += h->mapMimetypeIcon(doc.uri, doc.mimetype);
    icon += "'/>";
    map<string, string>::const_iterator t = doc.properties.find("title");
    size_t l = doc.uri.rfind('/');
    if (t != doc.properties.end()) {
        name = t->second.c_str();
    } else if (l != string::npos) {
        name = doc.uri.substr(l+1);
    } else {
        name = doc.uri;
    }
    if (l != string::npos) {
        folder = doc.uri.substr(0, l);
    } 
    out << "<div class='hit'>" << icon << "<h2><a href='" << link << "'>";
    out << name << "</a></h2><br/>score: ";
    out << doc.score << ", mime-type: " << doc.mimetype.c_str() << ", size: ";
    out << doc.size << ", last modified: " << h->formatDate(doc.mtime);
    string fragment = h->escapeString(doc.fragment.substr(0,100));
    out << "<br/><i>" << fragment << "</i><br/><table>";
    map<string, string>::const_iterator j;
    for (j = doc.properties.begin(); j != doc.properties.end(); ++j) {
        out << "<tr><td>" << j->first << ":</td><td>" << j->second.c_str()
            << "</td></tr>";
    }
    out << "</table></div>";
}
void
StrigiHtmlGui::Private::printSearchResults(std::ostream& out,
        const ClientInterface::Hits& hits) {
    std::vector<jstreams::IndexedDocument>::const_iterator i;
    for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
        printSearchResult(out, *i);
    }
}
