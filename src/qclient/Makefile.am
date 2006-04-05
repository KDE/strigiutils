INCLUDES = $(all_includes)

bin_PROGRAMS = qclient archivecat

qclient_SOURCES = filehandler.cpp filebrowser.cpp

qclient_CXXFLAGS = $(QT_CXXFLAGS) \
	-I$(top_srcdir)/archivereader/src/archiveengine
qclient_LDFLAGS = $(QT_LIBS)
qclient_LDADD = $(QT_LIBS) \
	$(top_builddir)/archivereader/src/streams/libstreams.la \
	$(top_builddir)/archivereader/src/archiveengine/libarchiveengine.la

archivecat_SOURCES = archivecat.cpp
archivecat_CXXFLAGS = $(QT_CXXFLAGS) \
	-I$(top_srcdir)/archivereader/src/archiveengine
archivecat_LDFLAGS = $(QT_LIBS)
archivecat_LDADD = $(QT_LIBS) \
	$(top_builddir)/archivereader/src/streams/libstreams.la \
	$(top_builddir)/archivereader/src/archiveengine/libarchiveengine.la

noinst_HEADERS = filebrowser.h

METASOURCES = AUTO
