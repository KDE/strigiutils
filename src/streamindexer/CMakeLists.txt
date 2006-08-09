subdirs (throughplugins)

include_directories( ../streams )

set(streamindexer_SRCS bz2endanalyzer.cpp digestthroughanalyzer.cpp 
	textendanalyzer.cpp streamendanalyzer.cpp zipendanalyzer.cpp
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp query.cpp
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp
	pluginthroughanalyzer.cpp moduleloader.cpp pluginendanalyzer.cpp
	arendanalyzer.cpp indexwriter.cpp)

if (Expat_FOUND)
  set(streamindexer_SRCS ${streamindexer_SRCS} expatsaxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${EXPAT_LIBRARY})
  include_directories(${EXPAT_INCLUDE_DIR})
else (Expat_FOUND)
  if (Libxml2_FOUND)
    set(streamindexer_SRCS ${streamindexer_SRCS} saxendanalyzer.cpp)
    set(streamindex_LIBS ${streamindex_LIBS} ${LIBXML2_LIBRARY})
    include_directories(${LIBXML2_INCLUDE_DIR})
  endif (Libxml2_FOUND)
endif(Expat_FOUND)

add_library(streamindexer SHARED ${streamindexer_SRCS})
set_target_properties(streamindexer PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

target_link_libraries(streamindexer streams ${streamindex_LIBS})

install(TARGETS streamindexer
	LIBRARY DESTINATION lib)
install(FILES indexeddocument.h indexwriter.h strigi_plugins.h
	streamthroughanalyzer.h streamendanalyzer.h
	DESTINATION include)


# test executable
add_executable(filelister filelistertest.cpp)

target_link_libraries(filelister streamindexer)
