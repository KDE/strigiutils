/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef DATAEVENTINPUTSTREAM_H
#define DATAEVENTINPUTSTREAM_H
#include "jstreamsconfig.h"
#include "streambase.h"

#include <list>

namespace jstreams {

/** Handler interface for handling data events. */
class STREAMS_EXPORT DataEventHandler {
public:
    virtual ~DataEventHandler() {}
    /** Handle a data event.
       @param data 
       @param size
       @return @c true  when the handler wants to receive more events
                  false when the handler does not want any more events from this
                        stream
     */
    virtual bool handleData(const char* data, uint32_t size) = 0;
    /** Handle the end of the stream. */
    virtual void handleEnd() {};
};

/** An InputStream that makes parallel handling of incoming data easier.
  When you read from this inputstream, it sends out data events to registered
  handlers. The data events cover all bytes in the inputstream and they are
  sent out in the same order as they occur in the stream. Each byte occurs in
  one event only. Rewinding this stream and rereading parts of it will not send
  a new event for the same data.
*/
class STREAMS_EXPORT DataEventInputStream : public StreamBase<char> {
private:
    int64_t totalread;
    StreamBase<char> *input;
    std::list<DataEventHandler*> handlers;

    void finish();
public:
    explicit DataEventInputStream(StreamBase<char> *input);
    void addEventHandler(DataEventHandler* );
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t reset(int64_t);
};

}
#endif
