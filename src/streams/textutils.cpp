#include "jstreamsconfig.h"
#include "textutils.h"


// http://www.w3.org/TR/REC-xml/#charsets
bool
jstreams::checkUtf8(const char* p, int32_t length) {
    const char* end = p + length;
    // check if the text is valid UTF-8
    char nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                return false;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            nb = 3;
        } else if (c < 0x20 && !(c == 0x9 || c == 0xA || c == 0xD)) {
            return false;
        }
        p++;
    }
    return true;
}

bool
jstreams::checkUtf8(const std::string& p) {
    return checkUtf8(p.c_str(), p.size());
}

/**
 * convert the \r and \n in utf8 strings into spaces
 **/
void
jstreams::convertNewLines(char* p) {
    int32_t len = strlen(p);
    bool ok = checkUtf8(p, len);
    if (!ok) fprintf(stderr, "string is not valid utf8\n");

    const char* end = p+len;
    char nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                return;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            nb = 3;
        } else if (c == '\n' || c == '\r') {
            *p = ' ';
        }
        p++;
    }
}


