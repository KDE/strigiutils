#include "indexer.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include "filereader.h"
#include "cluceneindexmanager.h"

using lucene::index::IndexWriter;
using namespace jstreams;

Indexer *Indexer::workingIndexer;

Indexer::Indexer(const char *indexdir) :m_indexdir(indexdir),
        m_manager(indexdir), m_writer(&m_manager), m_indexer(&m_writer) {
}
Indexer::~Indexer() {
    _lucene_shutdown();
}
void
Indexer::index(const char *dir) {
	workingIndexer = this;
	m_lister.setCallbackFunction(&Indexer::addFileCallback);
	bool exceptions = true;
	if (exceptions) {
		try {
			m_lister.listFiles(dir);
		} catch (CLuceneError& err) {
			printf(err.what());
		} catch(...) {
			printf("Unknown error");
		}
	} else {
		m_lister.listFiles(dir);
	}
}
bool
Indexer::addFileCallback(const string& path, const char *filename,
		time_t mtime) {
	workingIndexer->doFile(path+filename);
	return true;
}
void
Indexer::doFile(const std::string &filepath) {
	m_indexer.indexFile(filepath.c_str());
}
