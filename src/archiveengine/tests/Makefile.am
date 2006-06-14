INCLUDES = $(all_includes)

if HAVE_QTTEST
QT_PROGS = testrunner
TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh
endif

noinst_PROGRAMS = $(QT_PROGS)

check_PROGRAMS = $(QT_PROGS)

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_builddir)/testdata

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

testrunner_SOURCES = ArchiveEngineHandlerTest.cpp testrunner.cpp
nodist_testrunner_SOURCES = ArchiveEngineHandlerTest_moc.cpp
testrunner_CXXFLAGS = $(QTCORE_CFLAGS) $(QTTEST_CFLAGS) -I$(srcdir)/..
testrunner_LDFLAGS = $(QTCORE_LIBS) $(QTTEST_LIBS) -lQtTest
testrunner_LDADD = $(top_builddir)/src/archiveengine/libarchiveengine.la

noinst_HEADERS = ArchiveEngineHandlerTest.h

CLEANFILES = testrunner.cpp
