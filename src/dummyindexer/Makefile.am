bin_PROGRAMS = dummyindexer

dummyindexer_SOURCES = dummyindexer.cpp indexer.cpp
dummyindexer_LDADD = $(top_builddir)/src/streams/libstreams.la \
	$(top_builddir)/src/streamindexer/libstreamindexer.la
dummyindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer

noinst_HEADERS = indexer.h dummyindexwriter.h
