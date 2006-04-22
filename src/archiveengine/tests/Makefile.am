INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_srcdir)/archivereader/testdata


testrunner_SOURCES = ArchiveEngineHandlerTest.cpp testrunner.cpp
testrunner_CXXFLAGS = $(QT_CXXFLAGS) -I$(srcdir)/..
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest
testrunner_LDADD = $(LIB_QT) \
        $(top_builddir)/archivereader/src/archiveengine/libarchiveengine.la

METASOURCES = AUTO

noinst_HEADERS = ArchiveEngineHandlerTest.h

CLEANFILES = testrunner.cpp
