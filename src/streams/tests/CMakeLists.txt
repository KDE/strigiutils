
set(streamtests
	testrunner.cpp
	FileInputStreamTest.cpp
	ZipInputStreamTest.cpp
	GZipInputStreamTest.cpp
	BZ2InputStreamTest.cpp
	TarInputStreamTest.cpp
	RpmInputStreamTest.cpp
	SubInputStreamTest.cpp
	InputStreamTest.cpp
	InputStreamReaderTest.cpp
	MailInputStreamTest.cpp
	Base64InputStreamTest.cpp
	StringTerminatedSubStreamTest.cpp
	DigestInputStreamTest.cpp
)
if(NOT WIN32)
        set(streamtests ${streamtests}
                StringReaderTest.cpp    # this maybe works with mingw
                ArchiveReaderTest.cpp
                ProcessInputStreamTest.cpp
        )
endif(NOT WIN32)

CREATE_TEST_SOURCELIST(Tests ${streamtests})

ADD_EXECUTABLE(testrunner ${Tests} inputstreamtests.cpp)
target_link_libraries(testrunner streams)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun testrunner.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} testrunner ${TName} ${strigi_SOURCE_DIR}/testdata/data)
ENDFOREACH (test)

