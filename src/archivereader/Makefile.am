INCLUDES = $(all_includes)
SUBDIRS = . tests

lib_LTLIBRARIES = libarchiveengine.la

include_HEADERS = archiveenginehandler.h

libarchiveengine_la_SOURCES = fsfileinputstream.cpp streamengine.cpp \
	archivedirengine.cpp archiveengine.cpp archiveenginehandler.cpp 

libarchiveengine_la_CXXFLAGS = $(QTCORE_CFLAGS) $(QTGUI_CFLAGS) \
	-I$(top_srcdir)/src/streams
libarchiveengine_la_LDFLAGS = $(QTCORE_LIBS) $(QTGUI_LIBS) $(USER_LDFLAGS)
libarchiveengine_la_LIBADD = $(top_builddir)/src/streams/libstreams.la

noinst_HEADERS = archivedirengine.h archiveengine.h \
	fsfileinputstream.h streamengine.h archiveenginehandler.h
