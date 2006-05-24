INCLUDES = $(all_includes)

bin_PROGRAMS = daemon client listbuilder
lib_LTLIBRARIES = libsearchclient.la

daemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp
daemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/sqliteindexer
if HAVE_LINUXIOPRIO
daemon_CXXFLAGS += -DHAVE_LINUXIOPRIO
endif
daemon_LDFLAGS = -lpthread $(top_builddir)/src/sqliteindexer/libsqliteindex.la

client_SOURCES = client.cpp
client_LDFLAGS = libsearchclient.la

listbuilder_SOURCES = listbuilder.cpp
listbuilder_CXXFLAGS = -I$(top_srcdir)/src/streamindexer
listbuilder_LDFLAGS = $(top_builddir)/src/streamindexer/libstreamindexer.la

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS = clientinterface.h daemon.h interface.h server.h socketclient.h\
	socketserver.h
