lib_LTLIBRARIES = libstreams.la

libstreams_la_SOURCES = bz2inputstream.cpp digestinputstream.cpp \
	fileinputstream.cpp gzipinputstream.cpp \
	inputstreamreader.cpp filereader.cpp kmpsearcher.cpp \
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp \
	tarinputstream.cpp zipinputstream.cpp base64inputstream.cpp \
	dostime.c 

libstreams_la_CXXFLAGS =
libstreams_la_LDFLAGS = 
libstreams_la_LIBADD = -lz -lbz2 -lcrypto

SUBDIRS = . tests

noinst_HEADERS = base64inputstream.h bufferedstream.h bz2inputstream.h \
	digestinputstream.h dostime.h fileinputstream.h filereader.h \
	gzipinputstream.h inputstreambuffer.h inputstream.h inputstreamreader.h\
	jstreamsconfig.h kmpsearcher.h mailinputstream.h Reader.h streambase.h \
	stringreader.h stringterminatedsubstream.h subinputstream.h \
	substreamprovider.h tarinputstream.h zipinputstream.h
