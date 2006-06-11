#include "kittenhtmlgui.h"
#include "socketclient.h"
using namespace std;
using namespace jstreams;

class KittenHtmlGui::Private {
private:
    HtmlHelper* h;
public:
    SocketClient kitten;

    Private(HtmlHelper* h);
    void printSearchResult(std::ostream& out,
        const jstreams::IndexedDocument& doc);
    void printSearchResults(std::ostream& out, const ClientInterface::Hits&);
};

KittenHtmlGui::KittenHtmlGui(HtmlHelper* h) : helper(h) {
    p = new Private(helper);
}
KittenHtmlGui::~KittenHtmlGui() {
    delete p;
}
void
KittenHtmlGui::printPage(ostream&out,
        const std::map<std::string, std::string> &params) {

    std::string query;
    map<string, string>::const_iterator i = params.find("q");
    if (i != params.end()) query = i->second;

    out << "<html><title>Kitten Desktop Search</title></head><body>";

    out << "<h1>Kitten Desktop Search</h1>";

    out << "<form method='get'>";
    out << "<input type='text' name='q' value='" << query << "'/>";
    out << "<input type='submit'/></form>";

    const ClientInterface::Hits hits = p->kitten.query(query);
    p->printSearchResults(out, hits);

    out << "</body></html>";
}
KittenHtmlGui::Private::Private(HtmlHelper* helper) :h(helper) {
    string homedir = getenv("HOME");
    kitten.setSocketName(homedir+"/.kitten/socket");
}
void
KittenHtmlGui::Private::printSearchResult(std::ostream& out,
        const jstreams::IndexedDocument& doc) {
    string link, icon, name, folder;
    link = h->mapLinkUrl(doc.uri);
    icon = "<img src='";
    icon += h->mapMimetypeIcon(doc.mimetype);
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
    out << "<div class='hit'><h2><a href='" << link << "'>" << icon;
    out << " " << name << "</a></h2><br/>score: ";
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
KittenHtmlGui::Private::printSearchResults(std::ostream& out,
        const ClientInterface::Hits& hits) {
    std::vector<jstreams::IndexedDocument>::const_iterator i;
    for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
        printSearchResult(out, *i);
    }
}
