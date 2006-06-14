lib_LTLIBRARIES = libcluceneindex.la
bin_PROGRAMS = luceneindexer

libcluceneindex_la_SOURCES = cluceneindexwriter.cpp cluceneindexmanager.cpp \
	cluceneindexreader.cpp
libcluceneindex_la_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer $(CLUCENE_CXXFLAGS)
libcluceneindex_la_LIBADD = -lclucene $(top_builddir)/src/streams/libstreams.la\
	$(top_builddir)/src/streamindexer/libstreamindexer.la
luceneindexer_SOURCES = luceneindexer.cpp indexer.cpp

luceneindexer_LDFLAGS = libcluceneindex.la
luceneindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lclucene \
	$(top_builddir)/src/streamindexer/libstreamindexer.la -lpthread
luceneindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer  -fexceptions $(CLUCENE_CXXFLAGS)

noinst_HEADERS = cluceneindexmanager.h cluceneindexreader.h \
    cluceneindexwriter.h indexer.h
