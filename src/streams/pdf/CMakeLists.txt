
if (Magic_FOUND)
include_directories(.. ${MAGIC_INCLUDE_DIR})

add_executable(pdftest pdftest.cpp)

target_link_libraries(pdftest streams ${MAGIC_LIBRARIES})
endif (Magic_FOUND)

add_library(pdfstream pdfparser.cpp pdfinputstream.cpp)

add_executable(pdf pdf.cpp pdfparser.cpp)
target_link_libraries(pdf streams)
