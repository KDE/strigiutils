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
for field in xesamSessionFields:
    p = xesam.GetProperty(session, field)
    print field + ":\t" + dbusToStr(p)

#for i in range (100000):
print dbusToStr(xesam.SetProperty(session, "search.live", True))
print dbusToStr(xesam.SetProperty(session, "search.blocking", True))
print dbusToStr(xesam.SetProperty(session, "hit.fields", ["uri"]))
print dbusToStr(xesam.SetProperty(session, "hit.snippet.length", 30))
print dbusToStr(xesam.SetProperty(session, "sort.primary", "uri"))
print dbusToStr(xesam.SetProperty(session, "sort.secondary", "uri"))
print dbusToStr(xesam.SetProperty(session, "sort.order", "descending"))
print dbusToStr(xesam.SetProperty(session, "vendor.id", "whatever"))
print dbusToStr(xesam.SetProperty(session, "vendor.version", 3))
print dbusToStr(xesam.SetProperty(session, "vendor.display", "0wn3d"))
print dbusToStr(xesam.SetProperty(session, "vendor.xesam", "3"))
print dbusToStr(xesam.SetProperty(session, "vendor.fieldnames", ["uri", "hi"]))
print dbusToStr(xesam.SetProperty(session, "vendor.extensions", ["uri"]))

search = xesam.NewSearch(session, "<somexml/>")
xesam.StartSearch(search);

print session+"\t"+search
for i in range(1):
    xesam.GetHits(search, 3)
    xesam.CountHits(search)
print xesam.GetHits(search, 3)
print xesam.CountHits(search)
xesam.CloseSearch(search)
xesam.CloseSession(session)
