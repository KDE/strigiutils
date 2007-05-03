# TODO: fix the tests in the directory 'tests' and reenable them
subdirs(indexdump simplereader tests)

include_directories( ../streamanalyzer ../streams
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat
        ${CLUCENE_LIBRARY_DIR}
        ${CLUCENE_INCLUDE_DIR}
        ${ICONV_INCLUDE_DIR})

IF(WIN32)
    # why only here and not for the complete project?
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(WIN32)

# CLucene requires exception support and has no support for visibility=hidden
# so we must use the default (i.e. public) value for -fvisibility
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fvisibility=default")
ENDIF(NOT WIN32)

set(cluceneindex_SRCS
        PrefixFilter.cpp
        cluceneindexmanager.cpp
        cluceneindexreader.cpp
        cluceneindexwriter.cpp
        jsgzipcompressstream.cpp
)

add_library(cluceneindex SHARED ${cluceneindex_SRCS})

set_target_properties(cluceneindex
    PROPERTIES VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_CLUCENEINDEXER_LIB
)

target_link_libraries(cluceneindex streamanalyzer ${CLUCENE_LIBRARY})

add_executable(luceneindexer luceneindexer.cpp)
target_link_libraries(luceneindexer cluceneindex)

install(TARGETS luceneindexer RUNTIME DESTINATION bin)
install(TARGETS cluceneindex
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION  ${LIB_DESTINATION}
)
# FIXME: no need to install headers here?
