include(${QT_USE_FILE})

include_directories( ../streams )

add_library(archiveengine fsfileinputstream.cpp streamengine.cpp
        archivedirengine.cpp archiveengine.cpp archiveenginehandler.cpp)

target_link_libraries(archiveengine ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
	streams)
