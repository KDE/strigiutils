include_directories(../streamindexer ../htmlgui ../streams ../daemon
	qtdbus ${strigi_BINARY_DIR}/src/streams/compat filterwidget)
add_subdirectory(qtdbus)
add_subdirectory(filterwidget)

if (QT4_FOUND)
  include(${QT_USE_FILE})

  # strigiclient
  set(strigiclient_SRCS strigiclient.cpp simplesearchgui.cpp searchtabs.cpp
    searchview.cpp  dlglistindexedfiles.cpp dlgfilters.cpp histogram.cpp)
  set(strigiclient_MOC_HDRS searchtabs.h searchview.h simplesearchgui.h
    dlglistindexedfiles.h dlgfilters.h histogram.h)

  qt4_wrap_cpp(strigiclient_MOC_SRCS ${strigiclient_MOC_HDRS})

  add_executable(strigiclient ${strigiclient_SRCS} ${strigiclient_MOC_SRCS})

  target_link_libraries(strigiclient strigiqtdbusclient strigihtmlgui filterwidget)

  install(TARGETS strigiclient RUNTIME DESTINATION bin)
endif (QT4_FOUND)




