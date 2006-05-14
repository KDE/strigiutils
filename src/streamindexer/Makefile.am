lib_LTLIBRARIES = libstreamindexer.la

libstreamindexer_la_SOURCES = bz2endanalyzer.cpp \
	digestthroughanalyzer.cpp textendanalyzer.cpp \
	streamendanalyzer.cpp zipendanalyzer.cpp \
	tarendanalyzer.cpp streamindexer.cpp \
	mailendanalyzer.cpp gzipendanalyzer.cpp

libstreamindexer_la_CXXFLAGS = -I$(top_srcdir)/src/streams
libstreamindexer_la_LIBADD = $(top_builddir)/src/streams/libstreams.la

noinst_HEADERS = 
