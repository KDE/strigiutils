include(${QT_USE_FILE})

set(filterwidget_SRCS filterwidget.cpp filtermodel.cpp)
set(filterwidget_MOC_HDRS filterwidget.h)

qt4_wrap_cpp(filterwidget_MOC_SRCS ${filterwidget_MOC_HDRS})

add_library(filterwidget STATIC ${filterwidget_SRCS} ${filterwidget_MOC_SRCS})

target_link_libraries(filterwidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

add_executable(filterwidgettest filterwidgettest.cpp)

target_link_libraries(filterwidgettest filterwidget)
