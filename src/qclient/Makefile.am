INCLUDES = $(all_includes)

bin_PROGRAMS = qclient
qclient_SOURCES = filehandler.cpp filebrowser.cpp
# moc files
nodist_qclient_SOURCES =filebrowser_moc.cpp 

qclient_CXXFLAGS = $(QT_CXXFLAGS) -I$(top_srcdir)/src/archiveengine
qclient_LDFLAGS = $(QT_LIBS)
qclient_LDADD = $(QT_LIBS) $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/archiveengine/libarchiveengine.la

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

noinst_HEADERS = filebrowser.h
