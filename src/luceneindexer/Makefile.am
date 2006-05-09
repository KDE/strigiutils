
bin_PROGRAMS = luceneindexer
luceneindexer_SOURCES = luceneindexer.cpp indexer.cpp filelister.cpp

LUCENEDIR=/home/oever/tmp/testinstalls
luceneindexer_LDFLAGS = -L$(LUCENEDIR)/lib
luceneindexer_LDADD = $(top_builddir)/archivereader/src/streams/libstreams.la -lclucene
luceneindexer_CPPFLAGS = -I$(top_srcdir)/archivereader/src/streams -I$(LUCENEDIR)/include
noinst_HEADERS = 

KDE_CXXFLAGS=$(USE_EXCEPTIONS)
