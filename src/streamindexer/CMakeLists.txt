subdirs (throughplugins)

include_directories( ../streams )

set(streamindexer_SRCS bz2endanalyzer.cpp digestthroughanalyzer.cpp 
	textendanalyzer.cpp streamendanalyzer.cpp zipendanalyzer.cpp
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp query.cpp
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp
	pluginthroughanalyzer.cpp indexwriter.cpp)

if (FOUND_LIBXML2)
  set(streamindexer_SRCS ${streamindexer_SRCS} saxendanalyzer.cpp)
  set(streamindex_LIBS $(streamindex_LIBS) ${LIBXML2_LIBRARY})
  include_directories(${LIBXML2_INCLUDE_DIR})
endif (FOUND_LIBXML2)

if (FOUND_MAGIC)
  set(streamindexer_SRCS ${streamindexer_SRCS} mimetypethroughanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${MAGIC_LIBRARIES})
  include_directories(MAGIC_INCLUDE_DIR)
endif (FOUND_MAGIC)

add_library(streamindexer ${streamindexer_SRCS})

target_link_libraries(streamindexer streams ${streamindex_LIBS})
