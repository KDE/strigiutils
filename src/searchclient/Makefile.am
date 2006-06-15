INCLUDES = $(all_includes)

bin_PROGRAMS = strigiclient cmdlinestrigi

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

strigiclient_CXXFLAGS = $(QTCORE_CFLAGS) $(QTGUI_CFLAGS) \
	-I$(top_srcdir)/src/daemon -I$(top_srcdir)/src/streamindexer
strigiclient_LDFLAGS = $(QTCORE_LIBS) $(QTGUI_LIBS)
strigiclient_LDADD = $(top_builddir)/src/daemon/libsearchclient.la
strigiclient_SOURCES = strigiclient.cpp simplesearchgui.cpp \
	qt4strigiclient.cpp searchtabs.cpp searchview.cpp
nodist_strigiclient_SOURCES = simplesearchgui_moc.cpp \
	qt4strigiclient_moc.cpp searchtabs_moc.cpp searchview_moc.cpp

cmdlinestrigi_CXXFLAGS = -I$(top_srcdir)/src/daemon \
	-I$(top_srcdir)/src/streamindexer
cmdlinestrigi_LDADD = $(top_builddir)/src/daemon/libsearchclient.la
cmdlinestrigi_SOURCES = cmdlinestrigi.cpp

noinst_HEADERS = simplesearchgui.h searchtabs.h qt4strigiclient.h searchview.h
