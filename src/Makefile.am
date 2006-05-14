if HAVE_QTLIB
QT_SUBDIRS = archiveengine qclient
endif
if HAVE_CLUCENELIB
LUCENE_SUBDIRS = luceneindexer
endif
if HAVE_OPENSSL
INDEXER_SUBDIRS = streams streamindexer diskindexer sqliteindexer
endif
SUBDIRS = $(INDEXER_SUBDIRS) $(QT_SUBDIRS) $(LUCENE_SUBDIRS)

