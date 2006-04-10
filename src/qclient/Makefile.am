INCLUDES = $(all_includes)

bin_PROGRAMS = qclient archivecat archivedialog

CXXFLAGS = $(QT_CXXFLAGS) \
	-I$(top_srcdir)/archivereader/src/archiveengine
LDFLAGS = $(QT_LIBS)
LDADD = $(QT_LIBS) \
	$(top_builddir)/archivereader/src/archiveengine/libarchiveengine.la

qclient_SOURCES = filehandler.cpp filebrowser.cpp

archivecat_SOURCES = archivecat.cpp

archivedialog_SOURCES = archivedialog.cpp

noinst_HEADERS = filebrowser.h

METASOURCES = AUTO
