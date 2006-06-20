#include <pluginthroughanalyzer.h>

class XattrAnalyzer : public jstreams::StreamThroughAnalyzer {
private:
    static const int maxnamesize = 262144;
    int namesize;
    char* namebuffer;
    static const int maxvalsize = 262144;
    int valsize;
    char* valbuffer;
    jstreams::Indexable* idx;

    const char* retrieveAttribute(const char*);
public:
    XattrAnalyzer() {
        namebuffer = (char*)malloc(1024);
        namesize = 1024;
        valbuffer = (char*)malloc(1024);
        valsize = 1024;
    }
    ~XattrAnalyzer() {
        free(namebuffer);
        free(valbuffer);
    }
    void setIndexable(jstreams::Indexable*i) {
        idx = i;
    }
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

REGISTER_THROUGHANALYZER(XattrAnalyzer)

#include <sys/types.h>
#include <attr/xattr.h>

jstreams::InputStream *
XattrAnalyzer::connectInputStream(jstreams::InputStream *in) {
    ssize_t s;
    errno = 0;
    do {
        if (errno == ERANGE && namesize < maxnamesize) {
            namesize *= 2;
            namebuffer = (char*)realloc(namebuffer, namesize);
        }
        s = llistxattr(idx->getName().c_str(), namebuffer, namesize);
    } while (s == -1 && errno == ERANGE && namesize < maxnamesize);
    if (s == -1) return in;

    const char*start = namebuffer;
    const char*end = namebuffer;
    while (start-namebuffer < s) {
        if (*end == '\0') {
            if (end != start) {
                const char* val = retrieveAttribute(start);
                if (val) {
                    idx->setField(start, val);
                }
                start = end+1;
            }
        }
        end++;
    }

    return in;
}

const char*
XattrAnalyzer::retrieveAttribute(const char* name) {
    ssize_t s;
    errno = 0;
    do {
        if (errno == ERANGE && valsize < maxvalsize) {
            valsize *= 2;
            valbuffer = (char*)realloc(valbuffer, valsize);
        }
        s = lgetxattr(idx->getName().c_str(), name, valbuffer, valsize);
    } while (s == -1 && errno == ERANGE && valsize < maxvalsize);
    if (s == -1) return 0;
    return valbuffer;
}
