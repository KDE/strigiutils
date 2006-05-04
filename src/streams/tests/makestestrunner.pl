#! /usr/bin/perl -w
use strict;

# This script generates a test runner for the tests in this directory.
# Jos van den Oever

my $dir = $ARGV[0];
my $srcdir = $ARGV[1];

open(FH, "> testrunner.cpp") or die;

print FH <<THEEND
#include <QtTest/QtTest>
#ifdef HAVE_VALGRIND_H
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#else
#define VALGRIND_DO_QUICK_LEAK_CHECK
#define VALGRIND_COUNT_LEAKS(a,b,c,d)
#endif
int
test(QObject* o) {
    int errors;
    errors = QTest::qExec(o);
    delete o;
    VALGRIND_DO_QUICK_LEAK_CHECK;
    int leaked, dubious, reachable, suppressed;
    leaked = dubious = reachable = suppressed = 0;
    VALGRIND_COUNT_LEAKS(leaked, dubious, reachable, suppressed);
    return errors + leaked;
}
THEEND
;

foreach (glob("$dir/*Test.h")) {
	s#$dir/##;
	print FH "#include \"$_\"\n";
}

print FH "int main(int argc, char** argv) {\n\tint leaked = 0;\n";
print FH "\tbool all = argc == 1;\n\tfor (int i = (all)?0:1; i<argc; ++i) {\n";
print FH "\tQDir::setCurrent(\"$srcdir\");\n";

foreach (glob("$dir/*Test.h")) {
	s#$dir/##;
        s#.h$##;
	print FH "\t\tif (all || strcmp(argv[i],\"$_\")==0)\n";
        print FH "\t\t\tleaked += test(new $_());\n";
}

print FH "\t}\treturn leaked;\n}\n";
