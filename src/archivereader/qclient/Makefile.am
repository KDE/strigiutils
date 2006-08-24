INCLUDES = $(all_includes)

bin_PROGRAMS = qclient archivecat archivedialog

AM_CXXFLAGS = $(QTCORE_CFLAGS) $(QTGUI_CFLAGS) \
	-I$(top_srcdir)/src/archiveengine
AM_LDFLAGS = $(QTCORE_LIBS) $(QTGUI_LIBS)
LDADD = $(top_builddir)/src/archiveengine/libarchiveengine.la

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

qclient_SOURCES = filehandler.cpp filebrowser.cpp
nodist_qclient_SOURCES = filebrowser_moc.cpp 

archivecat_SOURCES = archivecat.cpp

archivedialog_SOURCES = archivedialog.cpp

noinst_HEADERS = filebrowser.h
