INCLUDES = $(all_includes)

if HAVE_QTLIB
QT_PROGS = testrunner
TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh
endif

noinst_PROGRAMS = mailtester $(QT_PROGS)

check_PROGRAMS = $(QT_PROGS)

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_srcdir)/testdata

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

testrunner_SOURCES = ZipInputStreamTest.cpp \
	GZipInputStreamTest.cpp \
	TarInputStreamTest.cpp \
	FileInputStreamTest.cpp \
	SubInputStreamTest.cpp \
	InputStreamTest.cpp \
	StringReaderTest.cpp \
	InputStreamReaderTest.cpp \
	MailInputStreamTest.cpp \
	Base64InputStreamTest.cpp \
	StringTerminatedSubStreamTest.cpp \
        inputstreamtests.cpp \
	testrunner.cpp $(testrunner_MOCSOURCES)

testrunner_MOCSOURCES = ZipInputStreamTest_moc.cpp \
	GZipInputStreamTest_moc.cpp \
	TarInputStreamTest_moc.cpp \
	FileInputStreamTest_moc.cpp \
	SubInputStreamTest_moc.cpp \
	InputStreamTest_moc.cpp \
	StringReaderTest_moc.cpp \
	InputStreamReaderTest_moc.cpp \
	MailInputStreamTest_moc.cpp \
	Base64InputStreamTest_moc.cpp \
	StringTerminatedSubStreamTest_moc.cpp \
        inputstreamtests_moc.cpp

testrunner_CXXFLAGS = $(QT_CFLAGS)
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest
testrunner_LDADD = $(top_builddir)/src/streams/libstreams.la

mailtester_SOURCES = mailtester.cpp
mailtester_LDFLAGS =
mailtester_LDADD = $(top_builddir)/src/streams/libstreams.la

testrunner_METASOURCES = AUTO

CLEANFILES = testrunner.cpp

