set(streamtests
	testrunner.cpp
	ArInputStreamTest.cpp
	BZ2InputStreamTest.cpp
	CpioInputStreamTest.cpp
	EventInputStreamTest.cpp
	FileInputStreamTest.cpp
	GZipInputStreamTest.cpp
#	GZipCompressInputStreamTest.cpp
	InputStreamReaderTest.cpp
	InputStreamTest.cpp
	KmpSearcherTest.cpp
	LZMAInputStreamTest.cpp
	MailInputStreamTest.cpp
	OleInputStreamTest.cpp
	RpmInputStreamTest.cpp
	SdfInputStreamTest.cpp
#	SignatureInputStreamTest.cpp # disabled because that stream is not done
	StringTerminatedSubStreamTest.cpp
	SubInputStreamTest.cpp
	TarInputStreamTest.cpp
	ZipInputStreamTest.cpp
	stat64bitTest.cpp
)
if(NOT WIN32)
        set(streamtests ${streamtests}
                ArchiveReaderTest.cpp
                ProcessInputStreamTest.cpp
                StringStreamTest.cpp    # this maybe works with mingw
        )
endif(NOT WIN32)

CREATE_TEST_SOURCELIST(Tests ${streamtests})

ADD_EXECUTABLE(testrunner-streams ${Tests}
  ../sharedtestcode/inputstreamtests.cpp)
target_link_libraries(testrunner-streams streams)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun testrunner.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} testrunner-streams ${TName} ${libstreams_SOURCE_DIR}/tests/data)
ENDFOREACH (test)
