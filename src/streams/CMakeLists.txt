add_library(streams bz2inputstream.cpp digestinputstream.cpp 
	fileinputstream.cpp gzipinputstream.cpp 
	inputstreamreader.cpp filereader.cpp kmpsearcher.cpp 
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp 
	tarinputstream.cpp zipinputstream.cpp base64inputstream.cpp 
	dostime.cpp substreamproviderprovider.cpp)

target_link_libraries(streams bz2 z crypto)
