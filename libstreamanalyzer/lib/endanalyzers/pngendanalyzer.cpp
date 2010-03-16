/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Vincent Ricard <magic@magicninja.org>
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
#include "pngendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <time.h>

#include <strigi/analysisresult.h>
#include "textendanalyzer.h"
#include <strigi/subinputstream.h>
#include <strigi/fieldtypes.h>
#include <strigi/gzipinputstream.h>
#include <strigi/textutils.h>
using namespace std;
using namespace Strigi;

/*
// and for the colors
static const char* colors[] = {
  "Grayscale",
  "Unknown",
  "RGB",
  "Palette",
  "Grayscale/Alpha",
  "Unknown",
  "RGB/Alpha"
};
*/

static const char* interlaceModes[] = {
  "None",
  "Adam7"
};

const string
    typeFieldName(
	"http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
    fullnameFieldName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact");


void
PngEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    colorDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    colorModeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
    interlaceModeField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#interlaceMode");
    lastModificationTimeField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentLastModified");
    titleField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
    authorField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#creator");
    descriptionField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#description");
    copyrightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright");
    creationTimeField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
    softwareField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#generator");
    disclaimerField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer");
    // putting warning into comment field since it's the closest equivalent
    warningField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
    // PNG spec says Source is Device used to create the image
    sourceField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#model");
    commentField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
    typeField = reg.typeField;

    /* add the fields to the internal list of fields */
    addField(widthField);
    addField(heightField);
    addField(colorDepthField);
    addField(colorModeField);
    addField(compressionField);
    addField(interlaceModeField);
    addField(lastModificationTimeField);
    addField(titleField);
    addField(authorField);
    addField(descriptionField);
    addField(copyrightField);
    addField(creationTimeField);
    addField(softwareField);
    addField(disclaimerField);
    addField(warningField);
    addField(sourceField);
    addField(commentField);
    addField(typeField);
}

PngEndAnalyzer::PngEndAnalyzer(const PngEndAnalyzerFactory* f) :factory(f) {
    // XXX hack to workaround mktime
    // which takes care of the local time zone
    struct tm timeZone;
    timeZone.tm_sec = 0;
    timeZone.tm_min = 0;
    timeZone.tm_hour = 0;
    timeZone.tm_mday = 1;
    timeZone.tm_mon = 0;
    timeZone.tm_year = 70;
    timeZone.tm_isdst = 0;
    timeZoneOffset = mktime(&timeZone);
}
bool
PngEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char pngmagic[]
        = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    return headersize >= 29 && memcmp(header, pngmagic, 8) == 0;
}
signed char
PngEndAnalyzer::analyze(AnalysisResult& as, InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 12, 12);
    if (nread != 12) {
        // file is too small to be a png
        return -1;
    }

    // read chunksize and include the size of the type and crc (4 + 4)
    uint32_t chunksize = readBigEndianUInt32(c+8) + 8;
    if (chunksize > 1048576) {
        fprintf(stderr,"chunk too big: %u\n",chunksize);
        return -1;
    }
    nread = in->read(c, chunksize, chunksize);
    // the IHDR chunk should be the first
    if (nread != (int32_t)chunksize || strncmp(c, "IHDR", 4)) {
        return -1;
    }

    as.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");

    // read the png dimensions
    uint32_t width = readBigEndianUInt32(c+4);
    uint32_t height = readBigEndianUInt32(c+8);
    as.addValue(factory->widthField, width);
    as.addValue(factory->heightField, height);

    uint16_t type = c[13];
    uint16_t bpp = c[12];

    // the bpp are only per channel, so we need to multiply the with
    // the channel count
    switch (type) {
        case 0: break;           // Grayscale
        case 2: bpp = (uint16_t)(3*bpp); break; // RGB
        case 3: break;           // palette
        case 4: bpp = (uint16_t)(2*bpp); break; // grayscale w. alpha
        case 6: bpp = (uint16_t)(4*bpp); break; // RGBA

        default: // we don't get any sensible value here
            bpp = 0;
    }

    as.addValue(factory->colorDepthField, (uint32_t)bpp);
    /* //FIXME: either get rid of this or replace with NIE equivalent
    as.addValue(factory->colorModeField,
        (type < sizeof(colors)/sizeof(colors[0]))
                   ? colors[type] : "Unknown");

    as.addValue(factory->compressionField,
        (c[14] == 0) ? "Deflate" : "Unknown");
    */
    as.addValue(factory->interlaceModeField,
        ((uint)c[16] < sizeof(interlaceModes)/sizeof(interlaceModes[0]))
                   ? interlaceModes[(int)c[16]] : "Unknown");

    // read the rest of the chunks
    // TODO: check if we want a fast or complete analysis
    // read the type
    nread = in->read(c, 8, 8);
    while (nread == 8 && strncmp("IEND", c+4, 4)) {
        // get the size of the data block
        chunksize = readBigEndianUInt32(c);

        if (strncmp("tEXt", c+4, 4) == 0) {
            // TODO convert latin1 to utf8 and analyze the format properly
            SubInputStream sub(in, chunksize);
            analyzeText(as, &sub);
            sub.skip(chunksize);
        } else if (strncmp("zTXt", c+4, 4) == 0) {
            SubInputStream sub(in, chunksize);
            analyzeZText(as, &sub);
            sub.skip(chunksize);
        } else if (strncmp("iTXt", c+4, 4) == 0) {
            SubInputStream sub(in, chunksize);
            analyzeText(as, &sub);
            sub.skip(chunksize);
        } else if (strncmp("tIME", c+4, 4) == 0) {
            SubInputStream sub(in, chunksize);
            analyzeTime(as, &sub);
            sub.skip(chunksize);
        } else {
            nread = (int32_t)in->skip(chunksize);
            if (nread != (int32_t)chunksize) {
                fprintf(stderr, "could not skip chunk size %u\n", chunksize);
                return -1;
            }
        }
        in->skip(4); // skip crc
        nread = in->read(c, 8, 8);
    }
    if (nread != 8) {
        // invalid file or error
        fprintf(stderr, "bad end in %s\n", as.path().c_str());
        return -1;
    }

    return 0;
}
signed char
PngEndAnalyzer::analyzeText(Strigi::AnalysisResult& as,
        InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 80, 80);
    if (nread < 1) {
         return -1;
    }
    // find the \0
    int32_t nlen = 0;
    while (nlen < nread && c[nlen]) nlen++;
    if (nlen == nread) return -1;
    const string name(c, nlen); // do something with the name!
    in->reset(nlen+1);
    return addMetaData(name, as, in);
}
signed char
PngEndAnalyzer::analyzeZText(Strigi::AnalysisResult& as,
        InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 81, 81);
    if (nread < 1) {
         return -1;
    }
    // find the \0
    int32_t nlen = 0;
    while (nlen < nread && c[nlen]) nlen++;
    if (nlen == nread) return -1;
    const string name(c, nlen); // do something with the name!
    in->reset(nlen+2);
    GZipInputStream z(in, GZipInputStream::ZLIBFORMAT);
    return addMetaData(name, as, &z);
}
signed char
PngEndAnalyzer::analyzeTime(Strigi::AnalysisResult& as,
        Strigi::InputStream* in) {
    const char* chunck;
    int32_t nread = in->read(chunck, 7, 7);
    if (nread != 7) {
        return -1;
    }

    int16_t year = readBigEndianInt16(chunck);
    int8_t month = *(chunck+2);
    int8_t day = *(chunck+3);
    int8_t hour = *(chunck+4);
    int8_t minute = *(chunck+5);
    int8_t second = *(chunck+6);
    // check the data (the leap second is allowed)
    if (!(1 <= month && month <= 12
            && 1 <= day && day <= 31
            && 0 <= hour && hour <= 23
            && 0 <= minute && minute <= 59
            && 0 <= second && second <= 60)) {
        return -1;
    }
    // we want to store the date/time as a number of
    // seconds since Epoch (1970-01-01T00:00:00)
    struct tm dateTime;
    dateTime.tm_sec = second;
    dateTime.tm_min = minute;
    dateTime.tm_hour = hour;
    dateTime.tm_mday = day;
    dateTime.tm_mon = month-1;
    dateTime.tm_year = year-1900;
    dateTime.tm_isdst = 0;

    time_t sinceEpoch = mktime(&dateTime);
    if (sinceEpoch == (time_t)-1) {
        fprintf(stderr, "could not compute the date/time\n");
        return -1;
    }

    // FIXME the chunck is UTC but mktime use the local timezone :-(
    // so i have to add the offset of the local time zone
    // If someone has a better solution...
    time_t time = sinceEpoch + timeZoneOffset;
    as.addValue(factory->lastModificationTimeField, (uint32_t)time);

    return 0;
}
signed char
PngEndAnalyzer::addMetaData(const string& key,
        Strigi::AnalysisResult& as, InputStream* in) {
    // try to store 1KB (should we get more?)
    const char* b;
    int32_t nread = in->read(b, 1024, 0);
    if (in->status() == Error) {
        m_error = in->error();
        return -1;
    }
    if (0 < nread) {
        const string value(b, nread);
        if ("Title" == key) {
            as.addValue(factory->titleField, value);
        } else if ("Author" == key) {
	    string authorUri = as.newAnonymousUri();

            as.addValue(factory->authorField, authorUri);
	    as.addTriplet(authorUri, typeFieldName, contactClassName);
	    as.addTriplet(authorUri, fullnameFieldName, value);
        } else if ("Description" == key) {
            as.addValue(factory->descriptionField, value);
        } else if ("Copyright" == key) {
            as.addValue(factory->copyrightField, value);
        } else if ("Creation Time" == key) {
            // TODO we need to parse the date time
            // "[...]the date format defined in section 5.2.14 of RFC 1123[...]"
        } else if ("Software" == key) {
            as.addValue(factory->softwareField, value);
        } else if ("Disclaimer" == key) {
            as.addValue(factory->disclaimerField, value);
        } else if ("Warning" == key) {
            as.addValue(factory->warningField, value);
        } else if ("Source" == key) {
            as.addValue(factory->sourceField, value);
        } else if ("Comment" == key) {
            as.addValue(factory->commentField, value);
        }
    }
    return 0;
}
