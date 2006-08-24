include(${QT_USE_FILE})

include_directories( ../streams ../archiveengine ../archivereader)

# qclient
set(qclient_SRCS filehandler.cpp filebrowser.cpp)
set(qclient_MOC_HDRS filebrowser.h)
qt4_wrap_cpp(qclient_MOC_SRCS ${qclient_MOC_HDRS})
add_executable(qclient ${qclient_SRCS} ${qclient_MOC_SRCS})
target_link_libraries(qclient streams archivereader
    ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

