#subdirs(tests)

include_directories( ../streamindexer ../streams ../filters
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

if (CLucene_FOUND)
    add_definitions(-DHAVE_CLUCENE)
    set(combinedindex_LIBS ${combinedindex_LIBS} cluceneindex)
    include_directories( ../luceneindexer )
endif (CLucene_FOUND)

if (HyperEstraier_FOUND)
    add_definitions(-DHAVE_ESTRAIER)
    set(combinedindex_LIBS ${combinedindex_LIBS} estraierindex)
    include_directories( ../estraierindexer )
endif (HyperEstraier_FOUND)

add_library(combinedindex combinedindexmanager.cpp)

target_link_libraries(combinedindex streamindexer ${combinedindex_LIBS})

#add_executable(combinedindexer combinedindexer.cpp indexer.cpp)
#target_link_libraries(combinedindexer combinedindex filters)

