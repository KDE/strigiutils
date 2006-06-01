lib_LTLIBRARIES = libxapianindex.la
bin_PROGRAMS = xapianindexer

libxapianindex_la_SOURCES = xapianindexwriter.cpp xapianindexmanager.cpp \
	xapianindexreader.cpp
libxapianindex_la_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer
libxapianindex_la_LIBADD = -lxapian $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

xapianindexer_SOURCES = xapianindexer.cpp indexer.cpp
xapianindexer_LDFLAGS = libxapianindex.la
xapianindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lxapian \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
xapianindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer

noinst_HEADERS = indexer.h xapianindexmanager.h xapianindexreader.h \
	xapianindexwriter.h
