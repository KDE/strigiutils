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
#ifndef STRIGI_DATAEVENTINPUTSTREAM_H
#define STRIGI_DATAEVENTINPUTSTREAM_H
#include <strigi/strigiconfig.h>
#include <strigi/streambase.h>

namespace Strigi {

/**
 * @brief Handler interface for handling data events.
 *
 * By passing an implementation of this class to a
 * DataEventInputStream, it will receive the data
 * as it is read from the DataEventInputStream.
 *
 * See the DataEventInputStream documentation for
 * more information.
 */
class STREAMS_EXPORT DataEventHandler {
public:
    /** Destructor */
    virtual ~DataEventHandler() {}
    /**
     * @brief Handle a data event.
     *
     * Each piece of data from an InputStream attached to a
     * DataEventInputStream will be passed to this function
     * (in order) as it is read from the DataEventInputStream.
     *
     * When handleEnd is called, it is guaranteed that each
     * element in the input stream has been passed in exactly
     * one call to this function, and that the calls happened
     * in the same order as the the data occurred in the
     * InputStream.
     *
     * You should not call this function yourself.  It forms
     * part of an interface for the use of a DataEventInputStream.
     *
     * @param data pointer to the data from the inputstream
     * @param size the size of the data pointed to by @p data
     * @return @c true  when the handler wants to receive more events
     *            false when the handler does not want any more events from this
     *                  stream
     */
    virtual bool handleData(const char* data, uint32_t size) = 0;
    /**
     * @brief Handle the end of the stream.
     *
     * This function will be called exactly once, and notifies
     * the DataEventHandler that all the data from the stream
     * has been read and passed in exactly one call to handleData.
     */
    virtual void handleEnd() {}
};

/**
 * @brief An InputStream that makes parallel handling of incoming data easier.
 *
 * When you read from this InputStream, it sends out data events to a registered
 * handler. The data events cover all bytes in the inputstream and they are
 * sent out in the same order as they occur in the stream. Each byte occurs in
 * one event only. Rewinding this stream and rereading parts of it will not send
 * a new event for the same data.
 *
 * Multiple DataEventInputStreams can be linked in series to send events to
 * several handlers:
 * @code
 * DataEventHandler handler1, handler2, handler3;
 * InputStream inputStream;
 * DataEventInputStream handlerStream1(inputStream, handler1);
 * DataEventInputStream handlerStream2(handlerStream1, handler1);
 * DataEventInputStream handlerStream3(handlerStream2, handler1);
 * int nRead = handlerStream3.read(start, min, max);
 * @endcode
 */
class STREAMS_EXPORT DataEventInputStream : public InputStream {
private:
    int64_t totalread;
    InputStream* input;
    DataEventHandler& handler;
    bool finished;

    void finish();
public:
    /**
     * @brief Creates a DataEventInputStream with a given InputStream
     * as the data source.
     *
     * Each instance of a DataEventHandler should only be passed to
     * one DataEventInputStream.  Behaviour is undefined otherwise.
     *
     * @param input the InputStream to use as the data source
     * @param handler the DataEventHandler that should be sent the
     * data events
     */
    explicit DataEventInputStream(InputStream *input,
        DataEventHandler& handler);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t reset(int64_t pos);
};

} // end namespace Strigi

#endif
