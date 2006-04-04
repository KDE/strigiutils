#include "indexer.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>

using lucene::analysis::standard::StandardAnalyzer;
using lucene::index::IndexReader;
using lucene::index::IndexWriter;
using lucene::index::Term;
using lucene::document::Document;
using lucene::document::Field;
using lucene::util::FileReader;

Indexer *Indexer::workingIndexer;

Indexer::Indexer(const char *indexdir) :m_indexdir(indexdir) {
	m_newIndex = !IndexReader::indexExists(indexdir);
	if (m_newIndex) {
		openWriter(true);
	} else {
		m_batchSize = 10;
		openReader();
	}
}
Indexer::~Indexer() {
	if (m_newIndex) {
		closeWriter();
	} else {
		closeReader();
		// add files left in batchbuffer
		addFiles();
	}
	_lucene_shutdown();
}
void
Indexer::openReader() {
	m_reader = IndexReader::open(m_indexdir.c_str());
}
void
Indexer::closeReader() {
	m_reader->close();
	delete m_reader;
}
void
Indexer::openWriter(bool create) {
	m_analyzer = new StandardAnalyzer();
	m_writer = new IndexWriter::IndexWriter(m_indexdir.c_str(), m_analyzer,
		create);
	m_writer->setMaxFieldLength(IndexWriter::DEFAULT_MAX_FIELD_LENGTH);
}
void
Indexer::closeWriter() {
	m_writer->optimize();
        m_writer->close();
	delete m_writer;
	delete m_analyzer;
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
void
Indexer::addFileCallback(const char *path, const char *filename) {
	std::string filepath(path);
	filepath += filename;
	workingIndexer->doFile(filepath);
}
void
Indexer::doFile(const std::string &filepath) {
	if (m_newIndex) {
		addFile(filepath);
	} else {
		deleteFile(filepath);
		m_toIndex.push_back(filepath);
		if (m_toIndex.size() == m_batchSize) {
			closeReader();
			addFiles();
			openReader();
		}
	}
}
void
Indexer::deleteFile(const std::string &filepath) {
	TCHAR tf[CL_MAX_DIR];
	STRCPY_AtoT(tf, filepath.c_str(), CL_MAX_DIR);
	Term term(_T("path"), tf);
	int n = m_reader->deleteTerm(&term);
	if (n) {
		printf("deleted %s %i times\n", filepath.c_str(), n);
	}
}
void
Indexer::addFile(const std::string &filepath) {
	TCHAR tf[CL_MAX_DIR];

	printf("adding %s\n", filepath.c_str());

    Document* doc = new Document();

	// add file name
	STRCPY_AtoT(tf, filepath.c_str(), CL_MAX_DIR);
	doc->add( *Field::Keyword(_T("path"), tf) );
	// add file content
        FileReader* reader = new FileReader(filepath.c_str(),
		PLATFORM_DEFAULT_READER_ENCODING);
        doc->add( *Field::Text(_T("contents"),reader) );
	// add to index
	m_writer->addDocument(doc);
	delete doc;
}
void
Indexer::addFiles() {
	openWriter();
	for (uint i=0; i<m_toIndex.size(); ++i) {
		addFile(m_toIndex[i]);
	}
	m_toIndex.clear();
	closeWriter();
}
