from gettext import gettext as _

import gnomevfs
import os
import re
import sys
from deskbar.Handler import Handler
from deskbar.Match import Match
import gtk, gtk.gdk

HANDLERS = {
        "LocateHandler" : {
                "name": _("Find Files by Name"),
                "description": _("Use locate to find files"),
        }
}

class LocateMatch(Match):
        def __init__(self, backend, dir=None, **args):
                Match.__init__(self, backend, **args)
                self.dir = dir
                self.url = dir+"/"+self.name

        def action(self, text=None):
                gnomevfs.url_show("file://"+self.url)

        def get_verb(self):
                return _("<b>%(name)s</b> "+self.url)

        def get_hash(self, text=None):
                return self.url

        def get_category(self):
                return "files"


class LocateHandler(Handler):
    def __init__(self):
        Handler.__init__(self, "gnome-mime-application-x-python.png")

    def query(self, query):
        result = []
        locate = os.popen('locate /'+query)
        test = "(.*)/("+query+"[^/]*)$"
        try:
            test = re.compile(test)
        except:
            return result
        line = locate.readline()
        while line and len(result) < 10:
            line = line.rstrip()
            m = test.match(line)
            if m:
                name = m.group(2)
                points = self.score(query, name)
                result.append(LocateMatch(self, name=name, dir=m.group(1)))
            line = locate.readline()
        return result

    def score(self, query, name):
        return len(name)

def main(args):
    h = LocateHandler()
    rs = h.query('abc')
    for r in rs:
        print r.name

if __name__=="__main__":
    main(sys.argv)
