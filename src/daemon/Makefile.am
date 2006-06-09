INCLUDES = $(all_includes)

bin_PROGRAMS = kittendaemon
lib_LTLIBRARIES = libsearchclient.la

kittendaemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp \
	indexscheduler.cpp clientinterface.cpp
kittendaemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer
kittendaemon_LDFLAGS = -lpthread

libsearchclient_la_SOURCES = socketclient.cpp clientinterface.cpp \
	asyncsocket.cpp asyncsocketclient.cpp
libsearchclient_la_CXXFLAGS = -I$(top_srcdir)/src/streamindexer

if HAVE_CLUCENE
libsearchclient_la_CXXFLAGS += -DHAVE_CLUCENE
kittendaemon_CXXFLAGS += -DHAVE_CLUCENE -I$(top_srcdir)/src/luceneindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/luceneindexer/libcluceneindex.la
endif
if HAVE_SQLITE
libsearchclient_la_CXXFLAGS += -DHAVE_SQLITE
kittendaemon_CXXFLAGS += -DHAVE_SQLITE -I$(top_srcdir)/src/sqliteindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/sqliteindexer/libsqliteindex.la
endif
if HAVE_ESTRAIER
libsearchclient_la_CXXFLAGS += -DHAVE_ESTRAIER
kittendaemon_CXXFLAGS += -DHAVE_ESTRAIER -I$(top_srcdir)/src/estraierindexer \
	$(ESTRAIERCFLAGS)
kittendaemon_LDFLAGS += $(top_builddir)/src/estraierindexer/libestraierindex.la
endif
if HAVE_XAPIAN
libsearchclient_la_CXXFLAGS += -DHAVE_XAPIAN
kittendaemon_CXXFLAGS += -DHAVE_XAPIAN -I$(top_srcdir)/src/xapianindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/xapianindexer/libxapianindex.la
endif
if HAVE_LINUXIOPRIO
kittendaemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif

noinst_HEADERS = daemon.h interface.h server.h socketserver.h indexscheduler.h \
	asyncsocket.h
