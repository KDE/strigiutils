INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir)

ArchiveEngineHandlerTest_moc.cpp: ArchiveEngineHandlerTest
	$(MOC) -o ArchiveEngineHandlerTest_moc.cpp $(srcdir)/ArchiveEngineHandlerTest

testrunner_SOURCES = ArchiveEngineHandlerTest.cpp ArchiveEngineHandlerTest_moc.cpp testrunner.cpp
testrunner_CXXFLAGS = $(QT_CXXFLAGS) -I$(srcdir)/..
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest_debug
testrunner_LDADD = $(LIB_QT) \
        $(top_builddir)/archivereader/src/archiveengine/libarchiveengine.la

METASOURCES = AUTO

noinst_HEADERS = ArchiveEngineHandlerTest
