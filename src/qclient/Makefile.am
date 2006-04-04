INCLUDES = $(all_includes)

lib_LTLIBRARIES = libarchiveengine.la

include_HEADERS = archiveenginehandler.h

libarchiveengine_la_SOURCES = fsfileinputstream.cpp streamengine.cpp \
	archivedirengine.cpp archiveengine.cpp archiveenginehandler.cpp 

libarchiveengine_la_CXXFLAGS = $(QT_CXXFLAGS) -I$(top_srcdir)/src/streams
libarchiveengine_la_LDFLAGS = $(QT_LDFLAGS) $(USER_LDFLAGS)
libarchiveengine_la_LIBADD = $(LIB_QT)

bin_PROGRAMS = qclient
qclient_SOURCES = filehandler.cpp filebrowser.cpp
# moc files
nodist_qclient_SOURCES =filebrowser_moc.cpp 

qclient_CXXFLAGS = $(QT_CXXFLAGS) -I$(top_srcdir)/src/streams
qclient_LDFLAGS = $(QT_LIBS)
qclient_LDADD = $(QT_LIBS) $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/qclient/libarchiveengine.la

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

noinst_HEADERS = archivedirengine.h archiveengine.h filebrowser.h \
	fsfileinputstream.h streamengine.h
