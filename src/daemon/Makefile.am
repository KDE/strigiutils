INCLUDES = $(all_includes)

bin_PROGRAMS = daemon client
lib_LTLIBRARIES = libsearchclient.la

daemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp
daemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/sqliteindexer
daemon_LDFLAGS = -lpthread $(top_builddir)/src/sqliteindexer/libsqliteindex.la

client_SOURCES = client.cpp socketclient.cpp

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS =

