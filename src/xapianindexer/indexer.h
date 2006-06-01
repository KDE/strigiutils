#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "filelister.h"
#include "xapianindexmanager.h"
#include "streamindexer.h"

class Indexer {
private:
	FileLister m_lister;
	const std::string m_indexdir;
	XapianIndexManager m_manager;
	jstreams::StreamIndexer m_indexer;

	static bool addFileCallback(const std::string& path,
        const char *filename, time_t mtime);
	static Indexer *workingIndexer;
	void doFile(const std::string &filepath);
public:
	Indexer(const char *indexdir);
	~Indexer();
	void index(const char *dir);
};

#endif
