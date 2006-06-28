#ifndef STRIGIHTMLGUI_H
#define STRIGIHTMLGUI_H

#include <ostream>
#include <map>
#include <string>

class HtmlHelper {
public:
    virtual ~HtmlHelper() {}
    /**
     * Function that maps a url for a retrieved file to a link
     * that is shown on the results page.
     **/
    virtual std::string mapLinkUrl(const std::string& url) = 0;
    /**
     * Function that maps mimetype onto an image url that points to
     * an image portraying the mimetype.
     **/
    virtual std::string mapMimetypeIcon(const std::string& url,
        const std::string& mimetype) = 0;
    /**
     * Function that encodes string into an escaped string suitable
     * for use in a url.
     **/
    virtual std::string encodeString(const std::string& url) = 0;
    virtual std::string escapeString(const std::string& url) = 0;
    virtual std::string formatDate(time_t date) = 0;
    virtual std::string getCssUrl() = 0;
};

class StrigiHtmlGui {
class Private;
private:
    HtmlHelper* helper;
    Private* p;

    void printHeader(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printMenu(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printFooter(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printConfig(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printSearch(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printHelp(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printAbout(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printStatus(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printIndexedDirs(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
public:
    StrigiHtmlGui(HtmlHelper*);
    ~StrigiHtmlGui();
    void printPage(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
};

#endif
