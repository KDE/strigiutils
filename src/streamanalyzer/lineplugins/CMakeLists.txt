include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams/compat)

add_library(xpm MODULE xpmlineanalyzer.cpp)
set_target_properties(xpm PROPERTIES
  PREFIX strigila_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(xpm streamanalyzer)
install(TARGETS xpm LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

add_library(deb MODULE deblineanalyzer.cpp)
set_target_properties(deb PROPERTIES
  PREFIX strigila_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(deb streamanalyzer)
install(TARGETS deb LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

add_library(cpp MODULE cpplineanalyzer.cpp)
set_target_properties(cpp PROPERTIES
  PREFIX strigila_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(cpp streamanalyzer)
install(TARGETS cpp LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
