INCLUDES = $(all_includes)

if HAVE_QTTEST
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
	DigestInputStreamTest.cpp \
        inputstreamtests.cpp \
	testrunner.cpp

nodist_testrunner_SOURCES = ZipInputStreamTest_moc.cpp \
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
	DigestInputStreamTest_moc.cpp \
        inputstreamtests_moc.cpp

testrunner_CXXFLAGS = $(QTCORE_CFLAGS) $(QTTEST_CFLAGS)
testrunner_LDFLAGS = $(QTCORE_LIBS) $(QTTEST_LIBS)
testrunner_LDADD = $(top_builddir)/src/streams/libstreams.la

mailtester_SOURCES = mailtester.cpp
mailtester_LDFLAGS =
mailtester_LDADD = $(top_builddir)/src/streams/libstreams.la

CLEANFILES = testrunner.cpp

noinst_HEADERS = Base64InputStreamTest.h DigestInputStreamTest.h \
	FileInputStreamTest.h GZipInputStreamTest.h InputStreamReaderTest.h \
	InputStreamTest.h inputstreamtests.h MailInputStreamTest.h \
	StringReaderTest.h StringTerminatedSubStreamTest.h SubInputStreamTest.h\
	TarInputStreamTest.h ZipInputStreamTest.h

EXTRA_DIST = makestestrunner.pl valgrindtest.sh

