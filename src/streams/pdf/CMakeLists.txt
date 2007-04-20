
include_directories(..
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat
)
if (Magic_FOUND)
include_directories(${MAGIC_INCLUDE_DIR})

add_executable(pdftest pdftest.cpp)

target_link_libraries(pdftest streams ${MAGIC_LIBRARIES})
endif (Magic_FOUND)

add_library(pdfstream pdfparser.cpp)
target_link_libraries(pdfstream streams)

add_executable(pdf pdf.cpp pdfparser.cpp)
target_link_libraries(pdf streams)
