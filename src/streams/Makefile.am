lib_LTLIBRARIES = libstreams.la

libstreams_la_SOURCES = bz2inputstream.cpp digestinputstream.cpp \
	fileinputstream.cpp gzipinputstream.cpp \
	inputstreamreader.cpp filereader.cpp kmpsearcher.cpp \
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp \
	tarinputstream.cpp zipinputstream.cpp base64inputstream.cpp \
	dostime.cpp substreamproviderprovider.cpp

libstreams_la_CXXFLAGS =
libstreams_la_LDFLAGS = 
libstreams_la_LIBADD = -lz -lbz2 -lcrypto

SUBDIRS = . tests

include_HEADERS = streambase.h substreamprovider.h substreamproviderprovider.h \
	fileinputstream.h bufferedstream.h inputstreambuffer.h inputstream.h

noinst_HEADERS = base64inputstream.h bz2inputstream.h \
	digestinputstream.h dostime.h filereader.h \
	gzipinputstream.h inputstreamreader.h\
	jstreamsconfig.h kmpsearcher.h mailinputstream.h Reader.h \
	stringreader.h stringterminatedsubstream.h subinputstream.h \
	tarinputstream.h zipinputstream.h
