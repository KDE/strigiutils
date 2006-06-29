INCLUDES = $(all_includes)

lib_LTLIBRARIES = libstrigihtmlgui.la

libstrigihtmlgui_la_CXXFLAGS = -I$(top_srcdir)/src/daemon \
	-I$(top_srcdir)/src/streamindexer
libstrigihtmlgui_la_LDFLAGS =
libstrigihtmlgui_la_LIBADD = $(top_builddir)/src/daemon/libsearchclient.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
libstrigihtmlgui_la_SOURCES = strigihtmlgui.cpp

include_HEADERS = strigihtmlgui.h

noinst_HEADERS = 
