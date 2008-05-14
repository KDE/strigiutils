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
#ifndef STRIGIHTMLGUI_H
#define STRIGIHTMLGUI_H

#include <strigi/strigiconfig.h>
#include "clientinterface.h"
#include <ostream>
#include <map>
#include <string>
#include <vector>


class HtmlHelper {
public:
    virtual ~HtmlHelper() {}
    /**
     * Function that maps a url for a retrieved file to a link
     * that is shown on the results page.
     **/
    virtual std::string mapLinkUrl(const std::string& url, int depth = 0) = 0;
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
    virtual std::string highlight(const std::string& text,
        const std::vector<std::string>& queryterms) = 0;
    virtual std::string mimetypeDescription(const std::string& mimetype) const {
        return mimetype;
    }
    virtual std::string getPathCharacterSeparator() { return ""; }
};

class STRIGIHTMLGUI_EXPORT StrigiHtmlGui {
class Private;
private:
    HtmlHelper* helper;
    Private* const p;

    void printHtmlHeader(std::ostream& out);
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
    std::map<std::string, std::string> readTabQueries() const;
public:
    StrigiHtmlGui(HtmlHelper*);
    ~StrigiHtmlGui();
    void printPage(std::ostream& out, const std::string& path,
        const std::map<std::string, std::string> &params);
    void printSearchResults(std::ostream& out,
        const ClientInterface::Hits&, const std::string& query);
};

#endif
