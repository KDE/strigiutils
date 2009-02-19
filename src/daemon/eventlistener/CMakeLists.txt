IF(ENABLE_INOTIFY)
    set (inotify_SRC inotifylistener.cpp)
ENDIF(ENABLE_INOTIFY)

IF(FAM_FOUND AND ENABLE_FAM)
    set (fam_SRC famlistener.cpp)
    set (eventlistener_LIBS ${eventlistener_LIBS} ${FAM_LIBRARIES})
ENDIF(FAM_FOUND AND ENABLE_FAM)

INCLUDE_DIRECTORIES ( ../../streams/strigi ..
    ${strigi_BINARY_DIR}/src/streams/strigi
    ${strigi_SOURCE_DIR}/src/streams/strigi)

add_library(eventlistener STATIC
    event.cpp
    eventlistenerqueue.cpp
    pollinglistener.cpp
    fslistener.cpp	
    ${inotify_SRC}
    ${fam_SRC})

target_link_libraries(eventlistener ${eventlistener_LIBS})
