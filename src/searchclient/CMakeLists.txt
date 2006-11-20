include_directories(../daemon ../streamindexer ../htmlgui ../filters ../streams qtdbus)
add_subdirectory(qtdbus)

if (QT4_FOUND)
  include(${QT_USE_FILE})

  # strigiclient
  set(strigiclient_SRCS strigiclient.cpp simplesearchgui.cpp searchtabs.cpp
    searchview.cpp dlgpreferences.cpp preferrorpage.cpp preffilterpage.cpp
    dlgaddfilteringrule.cpp dlglistindexedfiles.cpp)
  set(strigiclient_MOC_HDRS searchtabs.h searchview.h simplesearchgui.h
    dlgpreferences.h preferrorpage.h preffilterpage.h dlgaddfilteringrule.h
    dlglistindexedfiles.h)

  qt4_wrap_cpp(strigiclient_MOC_SRCS ${strigiclient_MOC_HDRS})

  add_executable(strigiclient ${strigiclient_SRCS} ${strigiclient_MOC_SRCS})

  target_link_libraries(strigiclient strigiqtdbusclient strigihtmlgui
    ${QT_QTGUI_LIBRARY})

  install(TARGETS strigiclient RUNTIME DESTINATION bin)
endif (QT4_FOUND)




