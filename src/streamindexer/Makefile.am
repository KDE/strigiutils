lib_LTLIBRARIES = libstreamindexer.la

libstreamindexer_la_SOURCES = bz2endanalyzer.cpp digestthroughanalyzer.cpp \
	textendanalyzer.cpp streamendanalyzer.cpp zipendanalyzer.cpp \
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp query.cpp\
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp indexreader.cpp \
	mimetypethroughanalyzer.cpp pluginthroughanalyzer.cpp indexwriter.cpp \
	saxendanalyzer.cpp

libstreamindexer_la_CXXFLAGS = -I$(top_srcdir)/src/streams $(LIBXML2_CFLAGS)
libstreamindexer_la_LIBADD = -lmagic $(top_builddir)/src/streams/libstreams.la \
	-lltdl $(LIBXML2_LIBS)

include_HEADERS = indexeddocument.h streamthroughanalyzer.h \
	pluginthroughanalyzer.h indexwriter.h

noinst_HEADERS = bz2endanalyzer.h digestthroughanalyzer.h filelister.h \
	gzipendanalyzer.h indexmanager.h indexreader.h indexwriter.h \
	mailendanalyzer.h pngendanalyzer.h streamendanalyzer.h streamindexer.h \
	tarendanalyzer.h textendanalyzer.h zipendanalyzer.h \
	mimetypethroughanalyzer.h saxendanalyzer.h

SUBDIRS = throughplugins 
