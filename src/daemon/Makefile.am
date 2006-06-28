INCLUDES = $(all_includes)

bin_PROGRAMS = strigidaemon
lib_LTLIBRARIES = libsearchclient.la

strigidaemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp \
	indexscheduler.cpp clientinterface.cpp
strigidaemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer
strigidaemon_LDFLAGS = -lpthread 

libsearchclient_la_SOURCES = socketclient.cpp clientinterface.cpp \
	asyncsocket.cpp asyncsocketclient.cpp
libsearchclient_la_CXXFLAGS = -I$(top_srcdir)/src/streamindexer

if HAVE_CLUCENE
libsearchclient_la_CXXFLAGS += -DHAVE_CLUCENE
strigidaemon_CXXFLAGS += -DHAVE_CLUCENE -I$(top_srcdir)/src/luceneindexer
strigidaemon_LDFLAGS += $(top_builddir)/src/luceneindexer/libcluceneindex.la
endif
if HAVE_SQLITE
libsearchclient_la_CXXFLAGS += -DHAVE_SQLITE
strigidaemon_CXXFLAGS += -DHAVE_SQLITE -I$(top_srcdir)/src/sqliteindexer
strigidaemon_LDFLAGS += $(top_builddir)/src/sqliteindexer/libsqliteindex.la
endif
if HAVE_ESTRAIER
libsearchclient_la_CXXFLAGS += -DHAVE_ESTRAIER
strigidaemon_CXXFLAGS += -DHAVE_ESTRAIER -I$(top_srcdir)/src/estraierindexer \
	$(ESTRAIERCFLAGS)
strigidaemon_LDFLAGS += $(top_builddir)/src/estraierindexer/libestraierindex.la
endif
if HAVE_XAPIAN
libsearchclient_la_CXXFLAGS += -DHAVE_XAPIAN
strigidaemon_CXXFLAGS += -DHAVE_XAPIAN -I$(top_srcdir)/src/xapianindexer
strigidaemon_LDFLAGS += $(top_builddir)/src/xapianindexer/libxapianindex.la
endif
if HAVE_LINUXIOPRIO
strigidaemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif

include_HEADERS = asyncsocketclient.h asyncsocket.h clientinterface.h

noinst_HEADERS = daemon.h interface.h server.h socketserver.h indexscheduler.h \
	asyncsocket.h socketclient.h
