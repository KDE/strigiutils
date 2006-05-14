
bin_PROGRAMS = sqliteindexer
sqliteindexer_SOURCES = sqliteindexer.cpp indexer.cpp filelister.cpp \
	sqliteindexwriter.cpp

sqliteindexer_LDFLAGS = 
sqliteindexer_LDADD = $(top_builddir)/src/streams/libstreams.la -lsqlite \
	$(top_builddir)/src/streamindexer/libstreamindexer.la

sqliteindexer_CPPFLAGS = -I$(top_srcdir)/src/streams \
	-I$(top_srcdir)/src/streamindexer  -fexceptions
noinst_HEADERS = 

