INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_srcdir)/archivereader/testdata

testrunner_SOURCES = ZipInputStreamTest.cpp \
	GZipInputStreamTest.cpp \
	TarInputStreamTest.cpp \
	FileInputStreamTest.cpp \
	SubInputStreamTest.cpp \
	InputStreamTest.cpp \
	StringReaderTest.cpp \
	InputStreamReaderTest.cpp \
        inputstreamtests.cpp \
	testrunner.cpp
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest
testrunner_LDADD = $(top_builddir)/archivereader/src/streams/libstreams.la

METASOURCES = AUTO

CLEANFILES = testrunner.cpp
