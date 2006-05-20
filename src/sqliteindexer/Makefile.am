
bin_PROGRAMS = sqliteindexer
lib_LTLIBRARIES = libsqliteindex.la

sqliteindexer_SOURCES = sqliteindexer.cpp indexer.cpp filelister.cpp \
	sqliteindexwriter.cpp sqliteindexmanager.cpp

sqliteindexer_LDFLAGS = 
sqliteindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lsqlite3 \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
sqliteindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer  -fexceptions

libsqliteindex_la_SOURCES = sqliteindexwriter.cpp sqliteindexmanager.cpp \
	sqliteindexreader.cpp
libsqliteindex_la_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer
libsqliteindex_la_LIBADD = -lsqlite3 $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

noinst_HEADERS = 

