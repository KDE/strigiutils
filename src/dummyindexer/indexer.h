#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "filelister.h"
#include "streamindexer.h"
#include "dummyindexwriter.h"

class Indexer {
private:
	FileLister m_lister;
	DummyIndexWriter writer;
	jstreams::StreamIndexer m_indexer;

	static bool addFileCallback(const char *path, const char *filename,
		time_t mtime);
	static Indexer *workingIndexer;
	void doFile(const std::string &filepath);
public:
	Indexer() :m_indexer(&writer) {}
	void index(const char *dir);
};

#endif
