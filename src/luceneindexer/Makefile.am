
bin_PROGRAMS = luceneindexer
luceneindexer_SOURCES = luceneindexer.cpp indexer.cpp \
	cluceneindexwriter.cpp cluceneindexreader.cpp cluceneindexmanager.cpp

luceneindexer_LDFLAGS = 
luceneindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lclucene \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

luceneindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer  -fexceptions
noinst_HEADERS = 

