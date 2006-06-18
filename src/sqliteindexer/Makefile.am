lib_LTLIBRARIES = libsqliteindex.la
bin_PROGRAMS = sqliteindexer

MODULELDFLAGS = @LIBLTDL@ -export-dynamic -lltdl

libsqliteindex_la_SOURCES = sqliteindexwriter.cpp sqliteindexmanager.cpp \
	sqliteindexreader.cpp
libsqliteindex_la_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer
libsqliteindex_la_LIBADD = -lsqlite3 $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

sqliteindexer_SOURCES = sqliteindexer.cpp indexer.cpp
sqliteindexer_LDFLAGS = libsqliteindex.la $(MODULELDFLAGS)
sqliteindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lsqlite3 \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
sqliteindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer

noinst_HEADERS = indexer.h sqliteindexmanager.h sqliteindexreader.h \
	sqliteindexwriter.h
