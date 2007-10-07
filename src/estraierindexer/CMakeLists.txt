subdirs(tests)

include_directories( ../streamanalyzer ../streams ${EST_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams
	${strigi_SOURCE_DIR}/src/streams/strigi)

add_library(estraierindex
	estraierindexmanager.cpp
        estraierindexreader.cpp
	estraierindexwriter.cpp
)

target_link_libraries(estraierindex streamanalyzer ${EST_LIBS})

add_executable(estraierindexer estraierindexer.cpp)
target_link_libraries(estraierindexer estraierindex)

