#include "indexreader.h"
using namespace std;
using namespace jstreams;

void
replaceall(string& text, const string& a, const string& b) {
    size_t pos = 0;
    pos = text.find(a);
    while (pos != string::npos) {
        text.replace(pos, a.length(), "&lt;");
        pos = text.find('<');
    }
}
string
Query::highlight(const string& text) const {
    return text;
    int pre = 5, post = 5, maxlen = 100;
    string t = text;
    replaceall(t, "<", "&lt;");
    replaceall(t, ">", "&gt;");
    string lt = t;
    for (uint i=0; i<t.length(); ++i) {
        lt[i] = tolower(lt[i]);
    }
    vector<string> re;
    map<string, set<string> >::const_iterator i;
    for (i = includes.begin(); i != includes.end(); ++i) {
        set<string>::const_iterator j;
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            string s = *j;
            for (int k = 0; k<s.length(); ++k) {
                s[k] = tolower(s[k]);
            }
            re.push_back(s);
        }
    }
    string out;
    int pos = 0;
    int last1 = string::npos;
    int last2 = string::npos;
    int last3 = string::npos;
    int last4 = string::npos;
    vector<string>::const_iterator k;
    while (pos >= 0 && out.length()+last1-last4 < maxlen) {
        int rep = string::npos;
        int len;
        for (k = re.begin(); k != re.end(); ++k) {
            int p = lt.find(*k, pos);
            if (p > 0 && (rep == string::npos || p < rep)) {
                rep = p;
                len = k->length();
            }
        }
        if (rep >= 0) {
            int p1 = t.find(" ", rep-pre);
            if (p1 == string::npos) p1 = (rep-pre < 0) ?0 : rep-pre;
            int p4 = t.find(" ", rep+len+post);
            if (p4 == string::npos) p4 = t.length();
            out += t.substr(p1, rep-p1);
            out += "<b>";
            out += t.substr(rep, len);
            out += "</b>";
            out += t.substr(rep+len, p4-(rep+len));
            out += " ... ";
            /* if (lasts == string::npos) {
                lasts = s;
            } else if (s > laste) {
                if (out.length() == 0 && lasts > 0) out += "... ";
                out += t.substr(lasts, laste - lasts) + " ... ";
                lasts = s;
            }
            laste = e;*/
            pos = rep+1;
        } else {
            pos = rep;
        }
    }
   /* if (lasts != string::npos) {
        if (out.length() == 0 && lasts > 0) out += "... ";
        out += t.substr(lasts, laste - lasts) + " ... ";
    }
    for (k = re.begin(); k != re.end(); ++k) {
        replaceall(out, *k, "<b>great</b>");
    }*/
    if (out.length() == 0) {
        out = t.substr(0, 100);
    }
    return out;
}
