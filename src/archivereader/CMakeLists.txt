include(${QT_USE_FILE})

include_directories( ../streams )

add_library(archivereader archivereader.cpp)

target_link_libraries(archivereader ${QT_QTCORE_LIBRARY} streams)

add_executable(archivecat archivecat.cpp)

target_link_libraries(archivecat streams archivereader)
