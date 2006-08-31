
INCLUDE_DIRECTORIES ( inotify ../streamindexer/ .. )

ADD_LIBRARY(eventlistener eventlistener.cpp event.cpp  eventlistenerqueue.cpp  inotifylistener.cpp)
