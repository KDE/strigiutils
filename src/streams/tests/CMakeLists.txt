find_package(Qt4)
include(${QT_USE_FILE})

# we seem to need at least Qt 4.1.3 to run the tests
if (QT_INST_MINOR_VERSION GREATER 0 
	AND QT_INST_PATCH_VERSION GREATER 2
	AND QT_QTTEST_FOUND) 
include_directories( ${QT_QTTEST_INCLUDE_DIR} )

ADD_CUSTOM_COMMAND(OUTPUT testrunner.cpp
	COMMAND perl ARGS
	${CMAKE_SOURCE_DIR}/src/streams/tests/makestestrunner.pl
	${CMAKE_CURRENT_SOURCE_DIR}/src/streams/tests
	${CMAKE_BUILD_DIR}/testdata
)

ADD_EXECUTABLE(testrunner ZipInputStreamTest.cpp 
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

endif (QT_INST_MINOR_VERSION GREATER 0 
	AND QT_INST_PATCH_VERSION GREATER 2
	AND QT_QTTEST_FOUND) 
