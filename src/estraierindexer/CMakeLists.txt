subdirs(tests)

include_directories( ../streamindexer ../streams ${EST_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams)

add_library(estraierindex estraierindexwriter.cpp estraierindexmanager.cpp
        estraierindexreader.cpp)

target_link_libraries(estraierindex streamindexer ${EST_LIBS})

add_executable(estraierindexer estraierindexer.cpp indexer.cpp)
target_link_libraries(estraierindexer estraierindex)

