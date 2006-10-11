#ifndef XMLSTREAM_H
#define XMLSTREAM_H

#include <string>

class SimpleNode;
class XMLStream {
private:
    class Private;
    Private* p;
public:
    XMLStream(const std::string& xml);
    ~XMLStream();
    void setFromAttribute(bool&, const char*);
    void setFromAttribute(int&, const char*);
    void setFromAttribute(std::string&, const char*);
    const std::string& getTagName() const;
    const SimpleNode& activeNode() const;
};

XMLStream& operator>>(XMLStream& in, bool& e);
XMLStream& operator>>(XMLStream& in, int& e);
XMLStream& operator>>(XMLStream& in, std::string& e);

#endif
