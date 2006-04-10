INCLUDES = $(all_includes)

AM_LDFLAGS = $(QT_LDFLAGS) -lQtTest_debug

check_PROGRAMS = testrunner

TESTS = $(check_PROGRAMS) $(top_srcdir)/archivereader/src/streams/tests/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(top_srcdir)/archivereader/src/streams/tests/makestestrunner.pl

testrunner_SOURCES = InputStreamTest.cpp testrunner.cpp

METASOURCES = AUTO
