if HAVE_QTLIB
QT_SUBDIRS = archiveengine qclient
endif
if HAVE_CLUCENELIB
LUCENE_SUBDIRS = luceneindexer
endif
if HAVE_OPENSSL
INDEXER_SUBDIRS = streamindexer diskindexer
endif
SUBDIRS = streams $(INDEXER_SUBDIRS) diskindexer $(QT_SUBDIRS) $(LUCENE_SUBDIRS)

