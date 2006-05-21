INCLUDES = $(all_includes)
SUBDIRS = . tests

lib_LTLIBRARIES = libarchiveengine.la

include_HEADERS = archiveenginehandler.h

libarchiveengine_la_SOURCES = fsfileinputstream.cpp streamengine.cpp \
	archivedirengine.cpp archiveengine.cpp archiveenginehandler.cpp 

libarchiveengine_la_CXXFLAGS = $(QT_CFLAGS) \
	-I$(top_srcdir)/src/streams
libarchiveengine_la_LDFLAGS = $(QT_LDFLAGS) $(USER_LDFLAGS)
libarchiveengine_la_LIBADD = $(LIB_QT) \
        $(top_builddir)/src/streams/libstreams.la

noinst_HEADERS = archivedirengine.h archiveengine.h \
	fsfileinputstream.h streamengine.h archiveenginehandler.h
