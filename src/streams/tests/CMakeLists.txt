find_package(Qt4)
include(${QT_USE_FILE})

ADD_CUSTOM_COMMAND(OUTPUT testrunner.cpp
	COMMAND perl ARGS
	$(CMAKE_SOURCE_DIR)/src/streams/tests/makestestrunner.pl
	$(CMAKE_CURRENT_SOURCE_DIR)/src/streams/tests
	$(CMAKE_BUILD_DIR)/testdata
)

ADD_EXECUTABLE(testrunner, ZipInputStreamTest.cpp 
	GZipInputStreamTest.cpp 
	TarInputStreamTest.cpp 
	FileInputStreamTest.cpp 
	SubInputStreamTest.cpp 
	InputStreamTest.cpp 
	StringReaderTest.cpp 
	InputStreamReaderTest.cpp 
	MailInputStreamTest.cpp 
	Base64InputStreamTest.cpp 
	StringTerminatedSubStreamTest.cpp 
	DigestInputStreamTest.cpp 
        inputstreamtests.cpp 
	testrunner.cpp)
