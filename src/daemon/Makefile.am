INCLUDES = $(all_includes)

bin_PROGRAMS = kittendaemon
lib_LTLIBRARIES = libsearchclient.la

kittendaemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp \
	indexscheduler.cpp
kittendaemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/sqliteindexer
if HAVE_LINUXIOPRIO
kittendaemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif
kittendaemon_LDFLAGS = -lpthread $(top_builddir)/src/sqliteindexer/libsqliteindex.la

client_SOURCES = client.cpp
client_LDFLAGS = libsearchclient.la

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS = clientinterface.h daemon.h interface.h server.h socketclient.h\
	socketserver.h indexscheduler.h
