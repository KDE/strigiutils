#include "eventthroughanalyzer.h"
#include "streamsaxanalyzer.h"
#include "streameventanalyzer.h"
#include "saxeventanalyzer.h"
#include "lineeventanalyzer.h"
#include "streamlineanalyzer.h"

using namespace std;
using namespace Strigi;

EventThroughAnalyzer::~EventThroughAnalyzer() {
    if (datastream) {
        delete datastream;
    }
    vector<StreamEventAnalyzer*>::iterator e;
    for (e = event.begin(); e != event.end(); ++e) {
        delete *e;
    }
}
void
EventThroughAnalyzer::setIndexable(AnalysisResult* r) {
    result = r;
}
InputStream*
EventThroughAnalyzer::connectInputStream(InputStream* in) {
    if (datastream) {
        delete datastream;
        datastream = 0;
    }
    if (event.size()) {
        datastream = new DataEventInputStream(in, *this);
        ready = false;
        vector<StreamEventAnalyzer*>::iterator i;
        for (i = event.begin(); i != event.end(); ++i) {
            (*i)->startAnalysis(result);
        }
    }
    return (datastream) ?datastream :in;
}
bool
EventThroughAnalyzer::isReadyWithStream() {
    return ready;
}
bool
EventThroughAnalyzer::handleData(const char* data, uint32_t size) {
    if (ready) return false;
    vector<StreamEventAnalyzer*>::iterator i;
    bool more = true;
    for (i = event.begin(); i != event.end(); ++i) {
        (*i)->handleData(data, size);
        more = more || !(*i)->isReadyWithStream();
    }
    ready = !more;
    return more;
}
void
EventThroughAnalyzer::handleEnd() {
    vector<StreamEventAnalyzer*>::iterator i;
    for (i = event.begin(); i != event.end(); ++i) {
        (*i)->endAnalysis();
    }
}
StreamThroughAnalyzer*
EventThroughAnalyzerFactory::newInstance() const {
    vector<StreamEventAnalyzer*> event;
    vector<StreamEventAnalyzerFactory*>::iterator ea;
    for (ea = eventfactories.begin(); ea != eventfactories.end(); ++ea) {
        event.push_back((*ea)->newInstance());
    }
    vector<StreamSaxAnalyzer*> sax;
    vector<StreamSaxAnalyzerFactory*>::iterator sa;
    for (sa = saxfactories.begin(); sa != saxfactories.end(); ++sa) {
        sax.push_back((*sa)->newInstance());
    }
    event.push_back(new SaxEventAnalyzer(sax));
    vector<StreamLineAnalyzer*> line;
    vector<StreamLineAnalyzerFactory*>::iterator la;
    for (la = linefactories.begin(); la != linefactories.end(); ++la) {
        line.push_back((*la)->newInstance());
    }
    event.push_back(new LineEventAnalyzer(line));
    StreamThroughAnalyzer* sta = new EventThroughAnalyzer(event);
    return sta;
}
void
EventThroughAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    vector<StreamEventAnalyzerFactory*>::iterator ea;
    for (ea = eventfactories.begin(); ea != eventfactories.end(); ++ea) {
        (*ea)->registerFields(reg);
    }
    vector<StreamSaxAnalyzerFactory*>::iterator sa;
    for (sa = saxfactories.begin(); sa != saxfactories.end(); ++sa) {
        (*sa)->registerFields(reg);
    }
    vector<StreamLineAnalyzerFactory*>::iterator la;
    for (la = linefactories.begin(); la != linefactories.end(); ++la) {
        (*la)->registerFields(reg);
    }
}
