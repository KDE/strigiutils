lib_LTLIBRARIES = libstreamindexer.la

libstreamindexer_la_SOURCES = bz2endanalyzer.cpp \
	digestthroughanalyzer.cpp \
	streamendanalyzer.cpp \
	tarendanalyzer.cpp streamindexer.cpp

libstreamindexer_la_CXXFLAGS = -I$(top_srcdir)/src/streams
libstreamindexer_la_LIBADD = -lcrypto \
	$(top_builddir)/src/streams/libstreams.la

noinst_HEADERS = 
