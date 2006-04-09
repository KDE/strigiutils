INCLUDES = $(all_includes)

AM_LDFLAGS = $(QT_LDFLAGS) -lQtTest_debug

check_PROGRAMS = inputstreamtest

TESTS = $(check_PROGRAMS) $(top_srcdir)/archivereader/src/streams/tests/valgrindtest.sh

inputstreamtest_SOURCES = inputstreamtest.cpp

METASOURCES = AUTO
