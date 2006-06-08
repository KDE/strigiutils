INCLUDES = $(all_includes)

bin_PROGRAMS = kittenclient cmdlinekitten

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

kittenclient_CXXFLAGS = $(QT_CFLAGS) -I$(top_srcdir)/src/daemon
kittenclient_LDFLAGS = $(QT_LIBS)
kittenclient_LDADD = $(top_builddir)/src/daemon/libsearchclient.la
kittenclient_SOURCES = kittenclient.cpp simplesearchgui.cpp queryexecuter.cpp \
	qt4kittenclient.cpp searchtabs.cpp searchview.cpp
nodist_kittenclient_SOURCES = simplesearchgui_moc.cpp queryexecuter_moc.cpp \
	qt4kittenclient_moc.cpp searchtabs_moc.cpp searchview_moc.cpp

cmdlinekitten_CXXFLAGS = -I$(top_srcdir)/src/daemon
cmdlinekitten_LDADD = $(top_builddir)/src/daemon/libsearchclient.la
cmdlinekitten_SOURCES = cmdlinekitten.cpp

noinst_HEADERS = simplesearchgui.h
