#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include <string>
#include <vector>

namespace jstreams {

template <class T>
class StreamBase;

/*
- create indexwriter
for all streams {
 - create an indexable
 - add the indexwriter to it
 - add a stream to the indexable (optional)
 - add fields to indexable (optional)
 - delete the indexable
}
- delete the indexwriter
*/
class Indexable;
class IndexWriter {
friend class Indexable;
protected:
    virtual void startIndexable(Indexable*) = 0;
    virtual void addText(const Indexable*, const char* text, int32_t length)=0;
    virtual void setField(const Indexable*, const std::string &fieldname,
        const std::string& value) = 0;
//    virtual void setField(const Indexable*, const std::string &fieldname,
//        int64_t value) = 0;
    virtual void finishIndexable(const Indexable*) = 0;
public:
    virtual ~IndexWriter() {}
    virtual void commit() { return; }
    virtual void deleteEntries(const std::vector<std::string>& entries) = 0;
    virtual int itemsInCache() { return 0; }
    virtual void optimize() {}
};

class Indexable {
friend class IndexWriter;
private:
    int64_t id;
    void* writerData;
    const int64_t mtime;
    const std::string& name;
    std::string encoding;
    IndexWriter* writer;
    char depth;
public:
    Indexable(const std::string& n, int64_t mt, IndexWriter* w, char d)
            :mtime(mt), name(n), writer(w), depth(d) {
        w->startIndexable(this);
    }
    ~Indexable() { writer->finishIndexable(this); }
    void addText(const char* text, int32_t length) {
        writer->addText(this, text, length);
    }
    void setField(const std::string& fieldname, const std::string& value) {
        writer->setField(this, fieldname, value);
    }
    const std::string& getName() const { return name; }
    int64_t getMTime() const { return mtime; }
    void setId(int64_t i) { id = i; }
    int64_t getId() const { return id; }
    char getDepth() const { return depth; }
    void setEncoding(const char* enc) { encoding = enc; }
    const char* getEncoding() const { return encoding.c_str(); }
    void* getWriterData() const { return writerData; }
    void setWriterData(void* wd) { writerData = wd; }
};


}

#endif
