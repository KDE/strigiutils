#! /usr/bin/perl -w
use strict;

# This script generates a test runner for the tests in this directory.
# Jos van den Oever

my $dir = $ARGV[0];
my $srcdir = $ARGV[1];

open(FH, "> testrunner.cpp") or die;

print FH <<THEEND
#include <QtTest/QtTest>
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
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

foreach (glob("$dir/*Test")) {
	s#$dir/##;
	print FH "#include \"$_\"\n";
}

print FH "int main() {\n\tint leaked = 0;\n";
print FH "\tQDir::setCurrent(\"$srcdir\");\n";

foreach (glob("$dir/*Test")) {
	s#$dir/##;
	print FH "\tleaked += test(new $_());\n";
}

print FH "\treturn leaked;\n}\n";
