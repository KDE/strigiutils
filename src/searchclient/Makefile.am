INCLUDES = $(all_includes)

bin_PROGRAMS = kittenclient

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

kittenclient_CXXFLAGS = $(QT_CFLAGS) -I$(top_srcdir)/src/daemon
kittenclient_LDFLAGS = $(QT_LIBS)
kittenclient_LDADD = $(top_builddir)/src/daemon/libsearchclient.la
kittenclient_SOURCES = kittenclient.cpp simplesearchgui.cpp \
	simplesearchgui_moc.cpp

noinst_HEADERS = 

