INCLUDES = $(all_includes)

lib_LTLIBRARIES = libkittenhtmlgui.la

libkittenhtmlgui_la_CXXFLAGS = -I$(top_srcdir)/src/daemon \
	-I$(top_srcdir)/src/streamindexer
libkittenhtmlgui_la_LDFLAGS =
libkittenhtmlgui_la_LIBADD = $(top_builddir)/src/daemon/libsearchclient.la
libkittenhtmlgui_la_SOURCES = kittenhtmlgui.cpp

include_HEADERS = kittenhtmlgui.h

noinst_HEADERS = 
