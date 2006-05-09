
bin_PROGRAMS = diskindexer
diskindexer_SOURCES = diskindex.cpp filelister.cpp

diskindexer_LDFLAGS = 
diskindexer_LDADD = $(top_builddir)/src/streamindexer/libstreamindexer.la
diskindexer_CXXFLAGS = -I$(top_srcdir)/src/streamindexer \
	-I$(top_srcdir)/src/streams
