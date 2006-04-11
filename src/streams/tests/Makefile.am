INCLUDES = $(all_includes)

check_PROGRAMS = testrunner

TESTS = $(check_PROGRAMS) $(srcdir)/valgrindtest.sh

testrunner.cpp: makestestrunner.pl
	perl $(srcdir)/makestestrunner.pl $(srcdir)

InputStreamTest_moc.cpp: InputStreamTest
	$(MOC) -o InputStreamTest_moc.cpp $(srcdir)/InputStreamTest

testrunner_SOURCES = InputStreamTest.cpp InputStreamTest_moc.cpp testrunner.cpp
testrunner_LDFLAGS = $(QT_LDFLAGS) -lQtTest

METASOURCES = AUTO
