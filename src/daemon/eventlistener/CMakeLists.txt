IF(ENABLE_INOTIFY)
    set (inotify_INCLUDE inotify)
    set (inotify_SRC inotifylistener.cpp)
ENDIF(ENABLE_INOTIFY)

INCLUDE_DIRECTORIES ( ${inotify_INCLUDE} ../../streams/compat .. 
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

ADD_LIBRARY(eventlistener eventlistener.cpp event.cpp  eventlistenerqueue.cpp  pollinglistener.cpp ${inotify_SRC})
