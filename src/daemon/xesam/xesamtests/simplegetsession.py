#! /usr/bin/env python
import dbus

# simple tests on the xesam interface
xesamSessionFields = ["search.live", "search.blocking", "hit.fields",
    "hit.snippet.length", "sort.primary", "sort.secondary", "sort.order",
    "vendor.id", "vendor.version", "vendor.display", "vendor.xesam",
    "vendor.fieldnames", "vendor.extensions", "nonsense.nonsense"]

# function to make a dbus object into a nice string
def dbusToStr(p):
    if not isinstance(p, dbus.String):
        try:
            p = str([str(string) for string in p])
        except:
            p = str(p)
    return p

# connect and print the status of the indexer
bus = dbus.SessionBus()
obj = bus.get_object('org.freedesktop.xesam.searcher',
	'/org/freedesktop/xesam/searcher/main')
xesam = dbus.Interface(obj, 'org.freedesktop.xesam.Search')
print xesam.GetState()

# create a session and print the properties of the new session
session = xesam.NewSession()
#for field in xesamSessionFields:
#    p = xesam.GetProperty(session, field)
#    print field + ":\t" + dbusToStr(p)

xesam.SetProperty(session, "hit.fields", ["system.location", "system.size",
    "content.mime_type", "system.depth", "system.last_modified_time"]
)

search = xesam.NewSearch(session, "<request><userQuery>\t    \n\tstrigi\t\tint</userQuery></request>")

for i in range(1):
    m = 0
    hits = xesam.GetHits(search, 100)
    for j in hits:
        n = 0
        for k in j:
            print str(m)+" "+str(n)+" "+str(k)
            n = n + 1
        m = m + 1
