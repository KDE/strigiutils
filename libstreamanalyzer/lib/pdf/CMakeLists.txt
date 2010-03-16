if (Magic_FOUND)
	add_executable(pdftest pdftest.cpp)
	target_link_libraries(pdftest STATIC streams ${MAGIC_LIBRARIES})
endif (Magic_FOUND)

add_library(pdfstream STATIC pdfparser.cpp)
target_link_libraries(pdfstream streams)

add_executable(pdf pdf.cpp pdfparser.cpp)
target_link_libraries(pdf streams)
