#include "indexer.h"
#include "filereader.h"

Indexer *Indexer::workingIndexer;

Indexer::Indexer(const char *indexdir) :m_indexdir(indexdir),
		m_writer(indexdir), m_indexer(&m_writer) {
}
Indexer::~Indexer() {
}
void
Indexer::index(const char *dir) {
	workingIndexer = this;
	m_lister.setCallbackFunction(&Indexer::addFileCallback);
	bool exceptions = true;
	if (exceptions) {
		try {
			m_lister.listFiles(dir);
		} catch(...) {
			printf("Unknown error");
		}
	} else {
		m_lister.listFiles(dir);
	}
}
void
Indexer::addFileCallback(const char *path, const char *filename) {
	std::string filepath(path);
	filepath += filename;
	workingIndexer->doFile(filepath);
}
void
Indexer::doFile(const std::string &filepath) {
	m_indexer.indexFile(filepath.c_str());
}
