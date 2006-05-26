INCLUDES = $(all_includes)

bin_PROGRAMS = kittendaemon
lib_LTLIBRARIES = libsearchclient.la

kittendaemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp \
	indexscheduler.cpp
kittendaemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/sqliteindexer
if HAVE_CLUCENELIB
kittendaemon_CXXFLAGS += -DHAVE_CLUCENELIB -I$(top_srcdir)/src/luceneindexer
endif
if HAVE_LINUXIOPRIO
kittendaemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif
kittendaemon_LDFLAGS = -lpthread \
	$(top_builddir)/src/sqliteindexer/libsqliteindex.la
if HAVE_CLUCENELIB
kittendaemon_LDFLAGS += $(top_builddir)/src/luceneindexer/libcluceneindex.la
endif

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS = clientinterface.h daemon.h interface.h server.h socketclient.h\
	socketserver.h indexscheduler.h
