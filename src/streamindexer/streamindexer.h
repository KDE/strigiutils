#ifndef STREAMINDEXER_H
#define STREAMINDEXER_H

#include <vector>

namespace jstreams {
class StreamEndAnalyzer;
class StreamThroughAnalyzer;
class IndexWriter;
template <class T> class StreamBase;
/**
 * The class StreamIndexer extracts information from files or character
 * streams. This task is performed by two separate analyzer classes:
 * - EndAnalyzer     pulls the data through the stream
 * - ThroughAnalyzer analyzes the data as it comes along
 * Examples for ThroughAnalyzers are classes that calculate a hash or
 * a checksum on a stream, or determine character usage.
 * An endanalyzer can do different things depending on the file type.
 * It could filter words from a text or split a stream up in substreams.
 * Especially the last task is important when working with archive streams
 * such as zip or tar files that contain other files.
 *
 * Currently this class is very simple and mainly useful for calculating
 * digests of streams in files in nested archives.
 **/
class StreamIndexer {
private:
    IndexWriter* writer;
    std::vector<std::vector<jstreams::StreamEndAnalyzer*> > end;
    std::vector<std::vector<jstreams::StreamThroughAnalyzer*> > through;

    void addThroughAnalyzers();
    void addEndAnalyzers();
    void removeIndexable(uint depth);
public:
    StreamIndexer(IndexWriter *w);
    ~StreamIndexer();
    char indexFile(const char *filepath);
    char indexFile(const std::string& filepath);
    char analyze(const std::string &path, int64_t mtime,
        jstreams::StreamBase<char> *input, uint depth);
};
}

#endif
