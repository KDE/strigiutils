IF(ENABLE_INOTIFY)
    set (inotify_INCLUDE inotify)
    set (inotify_SRC inotifylistener.cpp)
ENDIF(ENABLE_INOTIFY)

INCLUDE_DIRECTORIES ( ${inotify_INCLUDE} ../../streams/strigi .. 
	${strigi_BINARY_DIR}/src/streams/strigi
	${strigi_SOURCE_DIR}/src/streams/strigi)

ADD_LIBRARY(eventlistener
	event.cpp
	eventlistenerqueue.cpp
	pollinglistener.cpp
    fslistener.cpp	
	${inotify_SRC})
