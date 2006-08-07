INCLUDE_DIRECTORIES (inotify)

INCLUDE_DIRECTORIES ( ../streamindexer/ )

ADD_LIBRARY(eventlistener eventlistener.cpp event.cpp  eventlistenerqueue.cpp  inotifylistener.cpp)
