lib_LTLIBRARIES = libstreams.la

libstreams_la_SOURCES = bz2inputstream.cpp \
	fileinputstream.cpp gzipinputstream.cpp \
	inputstreamreader.cpp filereader.cpp kmpsearcher.cpp \
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp \
	tarinputstream.cpp zipinputstream.cpp dostime.c 

libstreams_la_CXXFLAGS =
libstreams_la_LDFLAGS = 
libstreams_la_LIBADD = -lz -lbz2

noinst_HEADERS = bufferedstream.h
SUBDIRS = tests
