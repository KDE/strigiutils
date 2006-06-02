lib_LTLIBRARIES = libestraierindex.la
bin_PROGRAMS = estraierindexer

libestraierindex_la_SOURCES = estraierindexwriter.cpp estraierindexmanager.cpp \
	estraierindexreader.cpp
libestraierindex_la_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer $(ESTRAIERCFLAGS)
libestraierindex_la_LIBADD = $(ESTRAIERLDFLAGS) \
	$(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

estraierindexer_SOURCES = estraierindexer.cpp indexer.cpp
estraierindexer_LDFLAGS = libestraierindex.la
estraierindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lestraier \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
estraierindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer

noinst_HEADERS = indexer.h estraierindexmanager.h estraierindexreader.h \
	estraierindexwriter.h
