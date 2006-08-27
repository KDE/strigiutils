if (QT4_FOUND)
    SUBDIRS(qclient)
endif (QT4_FOUND)

include_directories( ../streams )

add_executable(archivecat archivecat.cpp)

target_link_libraries(archivecat streams)

