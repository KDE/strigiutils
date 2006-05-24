#include "indexer.h"
#include "filereader.h"

Indexer *Indexer::workingIndexer;

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
bool
Indexer::addFileCallback(const char *path, const char *filename, time_t mtime) {
	std::string filepath(path);
	filepath += filename;
	workingIndexer->doFile(filepath);
	return true;
}
void
Indexer::doFile(const std::string &filepath) {
	m_indexer.indexFile(filepath.c_str());
}
