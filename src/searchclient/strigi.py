# This file is part of Strigi Desktop Search
#
# Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# A plugin for Gnome's Deskbar panel applet to talk to strigi.
# Can be tested on the command line by installing deskbar and run `python strigi.py <searchquery>`.
# Or for full use install to /usr/lib/deskbar-applet/handlers/ (or appropriate for your install) 
# and turning on the plugin in deskbar's preferences.

# Bugs: returns a bad result when strigidaemon is not running

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
                return _("<b>%(name)s</b> "+self.url)

        def get_hash(self, text=None):
                return self.url

        def get_category(self):
                return "files"

class StrigiHandler(Handler):
    def __init__(self):
        Handler.__init__(self, "gnome-fs-regular")
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
    rs = h.query(args[0])
    for r in rs:
        print r.url

if __name__=="__main__":
    main(sys.argv)
