#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "filelister.h"
#include "sqliteindexmanager.h"
#include "sqliteindexwriter.h"
#include "streamindexer.h"

class Indexer {
private:
	FileLister m_lister;
	const std::string m_indexdir;
	SqliteIndexManager m_manager;
	SqliteIndexWriter m_writer;
	jstreams::StreamIndexer m_indexer;

	static void addFileCallback(const char *path, const char *filename);
	static Indexer *workingIndexer;
	void doFile(const std::string &filepath);
public:
	Indexer(const char *indexdir);
	~Indexer();
	void index(const char *dir);
};

#endif
