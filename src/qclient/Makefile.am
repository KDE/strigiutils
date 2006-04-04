
bin_PROGRAMS = qclient
qclient_SOURCES = filehandler.cpp fsfileinputstream.cpp fsfileinputstream.h \
			archivedirengine.cpp archivedirengine.h archiveengine.cpp archiveengine.h streamengine.cpp \
			streamengine.h filebrowser.cpp archiveenginehandler.cpp
# moc files
nodist_qclient_SOURCES =filebrowser_moc.cpp 

qclient_CXXFLAGS = $(QT_CXXFLAGS) -I$(top_srcdir)/src/streams
qclient_LDFLAGS = $(QT_LIBS)
qclient_LDADD = $(QT_LIBS) $(top_builddir)/src/streams/libstreams.la

# rule to generate source files from headers
%_moc.cpp: %.h
	$(MOC) -o $@ $<

noinst_HEADERS = filebrowser.h archiveenginehandler.h
