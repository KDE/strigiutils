# allow use of internal api from shared test code
add_definitions(-DSTRIGI_INTERNAL_TEST -DSTREAMS_EXPORT=)

set(streamtests
	testrunner.cpp
	Base64InputStreamTest.cpp
	SkippingFileInputStreamTest.cpp
	MMapFileInputStreamTest.cpp
)

create_test_sourcelist(TESTS ${streamtests})

add_executable(testrunner-libstreams-internal ${TESTS}
	../sharedtestcode/inputstreamtests.cpp)
target_link_libraries(testrunner-libstreams-internal streamsstatic)

set(TESTSTORUN ${TESTS})
list(REMOVE_ITEM TESTSTORUN testrunner.cpp)

foreach(TEST ${TESTSTORUN})
	get_filename_component(TESTNAME ${TEST} NAME_WE)
	add_test(${TESTNAME} testrunner-libstreams-internal
		${TESTNAME} ${libstreams_SOURCE_DIR}/tests/data)
endforeach(TEST)
