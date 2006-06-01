INCLUDES = $(all_includes)

bin_PROGRAMS = kittendaemon
lib_LTLIBRARIES = libsearchclient.la

kittendaemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp \
	indexscheduler.cpp
kittendaemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer
kittendaemon_LDFLAGS = -lpthread
if HAVE_CLUCENE
kittendaemon_CXXFLAGS += -DHAVE_CLUCENE -I$(top_srcdir)/src/luceneindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/luceneindexer/libcluceneindex.la
endif
if HAVE_SQLITE
kittendaemon_CXXFLAGS += -DHAVE_SQLITE -I$(top_srcdir)/src/sqliteindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/sqliteindexer/libsqliteindex.la
endif
if HAVE_ESTRAIER
kittendaemon_CXXFLAGS += -DHAVE_ESTRAIER -I$(top_srcdir)/src/estraierindexer
kittendaemon_LDFLAGS += $(top_builddir)/src/estraierindexer/libestraierindex.la
endif
if HAVE_LINUXIOPRIO
kittendaemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS = clientinterface.h daemon.h interface.h server.h socketclient.h\
	socketserver.h indexscheduler.h
