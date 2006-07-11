subdirs (throughplugins)

include_directories( ../streams )

set(streamindexer_SRCS bz2endanalyzer.cpp digestthroughanalyzer.cpp 
	textendanalyzer.cpp streamendanalyzer.cpp zipendanalyzer.cpp
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp query.cpp
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp
	pluginthroughanalyzer.cpp indexwriter.cpp)

if (Expat_FOUND)
  set(streamindexer_SRCS ${streamindexer_SRCS} expatsaxendanalyzer.cpp)
  set(streamindex_LIBS $(streamindex_LIBS) ${EXPAT_LIBRARY})
  include_directories(${EXPAT_INCLUDE_DIR})
else (Expat_FOUND)
  if (Libxml2_FOUND)
    set(streamindexer_SRCS ${streamindexer_SRCS} saxendanalyzer.cpp)
    set(streamindex_LIBS $(streamindex_LIBS) ${LIBXML2_LIBRARY})
    include_directories(${LIBXML2_INCLUDE_DIR})
  endif (Libxml2_FOUND)
endif(Expat_FOUND)

add_library(streamindexer ${streamindexer_SRCS})

target_link_libraries(streamindexer streams ${streamindex_LIBS})

install(FILES indexeddocument.h indexwriter.h pluginthroughanalyzer.h
	streamthroughanalyzer.h
	DESTINATION include)
