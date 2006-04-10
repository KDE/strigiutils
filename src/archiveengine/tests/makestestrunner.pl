#! /usr/bin/perl -w
use strict;

# This script generates a test runner for the tests in this directory.
# Jos van den Oever

open(FH, "> testrunner.cpp") or die;

print FH <<THEEND
#include <QtTest/QtTest>
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
int
test(QObject* o) {
    QTest::qExec(o);
    delete o;
    VALGRIND_DO_QUICK_LEAK_CHECK;
    int leaked, dubious, reachable, suppressed;
    leaked = dubious = reachable = suppressed = 0;
    VALGRIND_COUNT_LEAKS(leaked, dubious, reachable, suppressed);
    return leaked;
}
THEEND
;

foreach (glob("*Test")) {
	print FH "#include \"$_\"\n";
}

print FH "int main() {\n\tint leaked = 0;\n";

foreach (glob("*Test")) {
	print FH "\tleaked += test(new $_());\n";
}

print FH "\treturn leaked;\n}\n";
