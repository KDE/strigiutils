#if LIB_QT
QT_SUBDIRS = archiveengine qclient
#endif
if HAVE_CLUCENE
LUCENE_SUBDIRS = luceneindexer
endif
SUBDIRS = streams streamindexer storage diskindexer $(QT_SUBDIRS) $(LUCENE_INDEXER)
