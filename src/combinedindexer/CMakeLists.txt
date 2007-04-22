#subdirs(tests)

include_directories( ../streamanalyzer ../streams ../filters ../dummyindexer
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

if (SQLite_FOUND)
    add_definitions(-DHAVE_SQLITE)
    set(combinedindex_LIBS ${combinedindex_LIBS} sqliteindex)
    include_directories( ../sqliteindexer )
endif (SQLite_FOUND)

add_library(combinedindex combinedindexmanager.cpp)

target_link_libraries(combinedindex streamanalyzer grepindex
    ${combinedindex_LIBS})

#add_executable(combinedindexer combinedindexer.cpp)
#target_link_libraries(combinedindexer combinedindex filters)

