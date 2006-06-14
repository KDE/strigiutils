if HAVE_QTGUI
QT_SUBDIRS = archiveengine qclient searchclient
endif
INDEXERS=
if HAVE_CLUCENE
INDEXERS += luceneindexer
endif
if HAVE_ESTRAIER
INDEXERS += estraierindexer
endif
if HAVE_XAPIAN
INDEXERS += xapianindexer
endif
if HAVE_SQLITE
INDEXERS += sqliteindexer
endif
if HAVE_OPENSSL
INDEXER_SUBDIRS = streams streamindexer dummyindexer
endif
SUBDIRS = $(INDEXER_SUBDIRS) $(INDEXERS) daemon $(QT_SUBDIRS) htmlgui

