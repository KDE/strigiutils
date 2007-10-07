include(${QT_USE_FILE})

include_directories(
    ../streams
    ${strigi_BINARY_DIR}/src/streams
	${BZIP2_INCLUDE_DIR}
	${QT_INCLUDE_DIR}
)

add_library(archiveengine
        archivedirengine.cpp
	archiveengine.cpp
	archiveenginehandler.cpp
	fsfileinputstream.cpp
	streamengine.cpp
)

target_link_libraries(archiveengine ${QT_QTCORE_LIBRARY} streams)
