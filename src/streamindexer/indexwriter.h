#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include <string>

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
    virtual void addStream(const Indexable*, const std::wstring& fieldname,
        jstreams::StreamBase<wchar_t>* datastream) = 0;
    virtual void addField(const Indexable*, const std::wstring &fieldname,
        const std::wstring &value) = 0;
    virtual void finishIndexable(const Indexable*) = 0;
public:
    virtual ~IndexWriter() {}
};

class Indexable {
private:
    IndexWriter* writer;
public:
    Indexable(IndexWriter* w) :writer(w) {}
    ~Indexable() { writer->finishIndexable(this); }
    void addStream(const std::wstring& fieldname,
            jstreams::StreamBase<wchar_t>* datastream) {
        writer->addStream(this, fieldname, datastream);
    }
    void addField(const std::wstring &fieldname,
            const std::wstring &value) {
        writer->addField(this, fieldname, value);
    }
};

}

#endif
