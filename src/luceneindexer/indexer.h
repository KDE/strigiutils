#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "filelister.h"

namespace lucene {
	namespace analysis {
		class Analyzer;
	}
	namespace index {
		class IndexReader;
		class IndexWriter;
	}
}
class Indexer {
private:
	bool m_newIndex;
	uint m_batchSize;
	FileLister m_lister;
	const std::string m_indexdir;
	std::vector<std::string> m_toIndex;

	lucene::analysis::Analyzer *m_analyzer;
	lucene::index::IndexReader *m_reader;
	lucene::index::IndexWriter *m_writer;

	static void addFileCallback(const char *path, const char *filename);
	static Indexer *workingIndexer;
	void doFile(const std::string &filepath);
	void deleteFile(const std::string &filepath);
	void addFile(const std::string &filepath);
	void addFiles();
	void openReader();
	void closeReader();
	void openWriter(bool create=false);
	void closeWriter();
public:
	Indexer(const char *indexdir);
	~Indexer();
	void index(const char *dir);
};

#endif
