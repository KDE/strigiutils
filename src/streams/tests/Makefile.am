INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_srcdir)/archivereader/testdata

InputStreamTest_moc.cpp: InputStreamTest
	$(MOC) -o InputStreamTest_moc.cpp $(srcdir)/InputStreamTest

FileInputStreamTest_moc.cpp: FileInputStreamTest
	$(MOC) -o FileInputStreamTest_moc.cpp $(srcdir)/FileInputStreamTest

ZipInputStreamTest_moc.cpp: ZipInputStreamTest
	$(MOC) -o ZipInputStreamTest_moc.cpp $(srcdir)/ZipInputStreamTest

testrunner_SOURCES = ZipInputStreamTest.cpp ZipInputStreamTest_moc.cpp \
	FileInputStreamTest.cpp FileInputStreamTest_moc.cpp \
	InputStreamTest.cpp InputStreamTest_moc.cpp \
	testrunner.cpp
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest
testrunner_LDADD = $(top_builddir)/archivereader/src/streams/libstreams.la

METASOURCES = AUTO

CLEANFILES = testrunner.cpp
