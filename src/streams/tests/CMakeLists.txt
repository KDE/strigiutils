
CREATE_TEST_SOURCELIST(Tests testrunner.cpp FileInputStreamTest.cpp
	ZipInputStreamTest.cpp
	GZipInputStreamTest.cpp
	BZ2InputStreamTest.cpp
	TarInputStreamTest.cpp
	SubInputStreamTest.cpp
	InputStreamTest.cpp
	StringReaderTest.cpp
	InputStreamReaderTest.cpp
	MailInputStreamTest.cpp
	Base64InputStreamTest.cpp
	StringTerminatedSubStreamTest.cpp
	DigestInputStreamTest.cpp
        ProcessInputStreamTest.cpp
)

ADD_EXECUTABLE(testrunner ${Tests} inputstreamtests.cpp)
target_link_libraries(testrunner streams)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun testrunner.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} testrunner ${TName} ${strigi_SOURCE_DIR}/testdata/data)
ENDFOREACH (test)

