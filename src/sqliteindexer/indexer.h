#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "filelister.h"
#include "sqliteindexmanager.h"
#include "streamindexer.h"

class Indexer {
private:
	FileLister m_lister;
	const std::string m_indexdir;
	SqliteIndexManager m_manager;
	jstreams::StreamIndexer m_indexer;

	static bool addFileCallback(const char *path, const char *filename);
	static Indexer *workingIndexer;
	void doFile(const std::string &filepath);
public:
	Indexer(const char *indexdir);
	~Indexer();
	void index(const char *dir);
};

#endif
