#include "inputstreamtest.h"
#include "valgrind.h"
#include "memcheck.h"

void
TestInputStream::initTestCase() {
}
void
TestInputStream::testStream() {
    QVERIFY(VALGRIND_COUNT_ERRORS == 0);
}

void
TestInputStream::cleanupTestCase() {
    VALGRIND_DO_QUICK_LEAK_CHECK;
    int leaked, dubious, reachable, suppressed;
    leaked = dubious = reachable = suppressed = 0;
    VALGRIND_COUNT_LEAKS(leaked, dubious, reachable, suppressed);
    QVERIFY(leaked == 0);
}

QTEST_APPLESS_MAIN(TestInputStream)
#include "inputstreamtest.moc"
