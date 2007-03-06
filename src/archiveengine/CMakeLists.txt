include(${QT_USE_FILE})

include_directories( ../streams ${BZIP2_INCLUDE_DIR})

add_library(archiveengine fsfileinputstream.cpp streamengine.cpp
        archivedirengine.cpp archiveengine.cpp archiveenginehandler.cpp)

target_link_libraries(archiveengine ${QT_QTCORE_LIBRARY} streams)
