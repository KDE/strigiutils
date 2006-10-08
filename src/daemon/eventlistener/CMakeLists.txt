IF(ENABLE_INOTIFY)
    set (inotify_INCLUDE inotify)
    set (inotify_SRC inotifylistener.cpp) 
ENDIF(ENABLE_INOTIFY)

INCLUDE_DIRECTORIES ( ${inotify_INCLUDE} ../streamindexer/ .. )

ADD_LIBRARY(eventlistener eventlistener.cpp event.cpp  eventlistenerqueue.cpp  pollinglistener.cpp ${inotify_SRC})
