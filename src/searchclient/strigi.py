from gettext import gettext as _

import gnomevfs
import os
import re
import sys
import dbus
import dbus.glib
from deskbar.Handler import Handler
from deskbar.Match import Match
import gtk, gtk.gdk

HANDLERS = {
        "StrigiHandler" : {
                "name": _("Find Files with Strigi"),
                "description": _("Use Strigi to find files"),
        }
}

class StrigiMatch(Match):
        def __init__(self, backend, dir=None, **args):
                Match.__init__(self, backend, **args)
                self.dir = dir
                self.url = dir+"/"+self.name

        def action(self, text=None):
                gnomevfs.url_show("file://"+self.url)

        def get_verb(self):
                if self.name == "Error":
                    return self.dir
                return _("<b>%(name)s</b>"+self.url)

        def get_hash(self, text=None):
                return self.url

        def get_category(self):
                return "files"

class StrigiHandler(Handler):
    def __init__(self):
        Handler.__init__(self, "gnome-mime-application-x-python.png")
        self.strigi = None

    def connect(self):
        # connect to DBus
        try:
            bus = dbus.SessionBus()
        except:
            self.error = "DBus is not running."
            return
        # make a connection to the strigi daemon
        try:
            obj = bus.get_object('vandenoever.strigi', '/search')
            self.strigi = dbus.Interface(obj, 'vandenoever.strigi')
            self.strigi.getStatus()
        except:
            self.strigi = None
            self.error = "Strigi is not running."

    def query(self, query):
        results = []
        # connect to Strigi
        if self.strigi == None:
            self.connect()
        if self.strigi == None:
            results.append(StrigiMatch(self, name="Error",
                dir="Strigi not available"))
            return results

        # perform the search
        try:
            hits = self.strigi.getHits(query, 10, 0)
        except:
            # ignore the error, just return no results
            results.append(StrigiMatch(self, name="Error",
                dir="Strigi timeout"))
            self.strigi = None
            return results

        # parse the results
        for hit in hits:
            test = re.compile("(.*)/([^/]*)$")
            m = test.match(hit[0])
            results.append(StrigiMatch(self, name=m.group(2), dir=m.group(1)))
        return results

def main(args):
    h = StrigiHandler()
    rs = h.query('strigi')
    for r in rs:
        print r.url

if __name__=="__main__":
    main(sys.argv)
