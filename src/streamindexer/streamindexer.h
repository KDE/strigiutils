#ifndef STREAMINDEXER_H
#define STREAMINDEXER_H

#include <vector>

#include "inputstream.h"

class StreamEndAnalyzer;
class StreamThroughAnalyzer;

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
    std::vector<std::vector<StreamEndAnalyzer*> > end;
    std::vector<std::vector<StreamThroughAnalyzer*> > through;

    void addThroughAnalyzers();
    void addEndAnalyzers();
public:
    StreamIndexer();
    ~StreamIndexer();
    char indexFile(const char *filepath);
    char indexFile(std::string& filepath);
    char analyze(std::string &path, jstreams::InputStream *input, uint depth);
};

#endif
