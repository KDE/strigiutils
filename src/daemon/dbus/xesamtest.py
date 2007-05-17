#! /usr/bin/env python
import dbus

# simple tests on the xesam interface

# connect
bus = dbus.SessionBus()
obj = bus.get_object('org.freedesktop.xesam.searcher',
	'/org/freedesktop/xesam/searcher/main')
xesam = dbus.Interface(obj, 'org.freedesktop.xesam.search')
print xesam.GetState()
session = xesam.NewSession()
print session
print xesam.SetProperty(session, "search.live", True)
print xesam.SetProperty(session, "search.blocking", True)
print xesam.SetProperty(session, "hit.fields", ["uri"])
print xesam.SetProperty(session, "hit.snippet.length", 30)
print xesam.SetProperty(session, "sort.primary", "uri")
print xesam.SetProperty(session, "sort.secondary", "uri")
print xesam.SetProperty(session, "sort.order", "descending")
print xesam.SetProperty(session, "vendor.id", "whatever")
print xesam.SetProperty(session, "vendor.version", 3)
print xesam.SetProperty(session, "vendor.display", "0wn3d")
print xesam.SetProperty(session, "vendor.xesam", "3")
print xesam.SetProperty(session, "vendor.fieldnames", ["uri", "hi"])
print xesam.SetProperty(session, "vendor.extensions", ["uri"])
print xesam.GetHits(session, 3)
