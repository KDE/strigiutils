INCLUDES = $(all_includes)

bin_PROGRAMS = daemon
lib_LTLIBRARIES = libsearchclient.la

daemon_SOURCES = daemon.cpp server.cpp socketserver.cpp interface.cpp

daemon_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/sqliteindexer
daemon_LDFLAGS = -lpthread $(top_builddir)/src/sqliteindexer/libsqliteindex.la

libsearchclient_la_SOURCES = socketclient.cpp 

noinst_HEADERS = clientinterface.h daemon.h interface.h server.h socketclient.h\
	socketserver.h
