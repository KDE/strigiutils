
include_directories( ../streams )

add_library(archivereader archivereader.cpp)

target_link_libraries(archivereader streams)

add_executable(archivecat archivecat.cpp)

target_link_libraries(archivecat streams archivereader)

