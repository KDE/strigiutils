lib_LTLIBRARIES = libstreamindexer.la

libstreamindexer_la_SOURCES = bz2endanalyzer.cpp \
	digestthroughanalyzer.cpp textendanalyzer.cpp \
	streamendanalyzer.cpp zipendanalyzer.cpp \
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp \
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp

libstreamindexer_la_CXXFLAGS = -I$(top_srcdir)/src/streams
libstreamindexer_la_LIBADD = $(top_builddir)/src/streams/libstreams.la

noinst_HEADERS = bz2endanalyzer.h digestthroughanalyzer.h filelister.h \
	gzipendanalyzer.h indexmanager.h indexreader.h indexwriter.h \
	mailendanalyzer.h pngendanalyzer.h streamendanalyzer.h streamindexer.h \
	streamthroughanalyzer.h tarendanalyzer.h textendanalyzer.h \
	zipendanalyzer.h
