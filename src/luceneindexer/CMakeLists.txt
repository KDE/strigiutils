subdirs(tests indexdump simplereader)

include_directories( ../streamindexer ../streams
	${CLUCENE_LIBRARY_DIR}
	${CLUCENE_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

IF(MSVC)
    # why only here and not for the complete project?
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(MSVC)

set(cluceneindex_SRCS
        cluceneindexwriter.cpp
        cluceneindexmanager.cpp
        cluceneindexreader.cpp
        PrefixFilter.cpp
)

add_library(cluceneindex SHARED ${cluceneindex_SRCS})

set_target_properties(cluceneindex PROPERTIES
    DEFINE_SYMBOL MAKE_CLUCENEINDEXER_LIB
)

target_link_libraries(cluceneindex streamindexer ${CLUCENE_LIBRARY})

add_executable(luceneindexer luceneindexer.cpp indexer.cpp)
target_link_libraries(luceneindexer cluceneindex)

install(TARGETS luceneindexer RUNTIME DESTINATION bin)
# FIXME: no need to install headers here?
