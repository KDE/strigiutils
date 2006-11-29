include_directories()

if (QT4_FOUND)
  include(${QT_USE_FILE})

  set(filterwidget_SRCS filterwidget.cpp filtermodel.cpp)
  set(filterwidget_MOC_HDRS filterwidget.h filtermodel.h)

  qt4_wrap_cpp(filterwidget_MOC_SRCS ${filterwidget_MOC_HDRS})

  add_executable(filterwidgettest filterwidgettest.cpp ${filterwidget_SRCS}
    ${filterwidget_MOC_SRCS})

  target_link_libraries(filterwidgettest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

endif (QT4_FOUND)




