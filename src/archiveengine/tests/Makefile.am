INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir) $(top_builddir)/testdata

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

testrunner_SOURCES = ArchiveEngineHandlerTest.cpp \
	ArchiveEngineHandlerTest_moc.cpp testrunner.cpp
testrunner_CXXFLAGS = $(QT_CFLAGS) -I$(srcdir)/..
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest
testrunner_LDADD = $(LIB_QT) \
        $(top_builddir)/src/archiveengine/libarchiveengine.la

noinst_HEADERS = ArchiveEngineHandlerTest.h

CLEANFILES = testrunner.cpp
