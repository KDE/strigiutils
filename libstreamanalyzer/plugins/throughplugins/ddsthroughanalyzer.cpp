/* This file is part of Strigi Desktop Search, ported from code of:
 * Copyright (C) 2002 Ignacio Castaño <castano@ludicon.com>
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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

#include "ddsthroughanalyzer.h"
#include <strigi/textutils.h>
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>

using namespace std;
using namespace Strigi;

// this analyzer reads the DirectDraw Surface graphics file format
// http://www.modwiki.net/wiki/DDS_(file_format)
// http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c/directx/graphics/reference/ddsfilereference/ddsfileformat.asp

namespace {// Private.
#if !defined(MAKEFOURCC)
#	define MAKEFOURCC(ch0, ch1, ch2, ch3) \
        (uint32_t(uint8_t(ch0)) | (uint32_t(uint8_t(ch1)) << 8) | \
        (uint32_t(uint8_t(ch2)) << 16) | (uint32_t(uint8_t(ch3)) << 24 ))
#endif

    static const uint32_t FOURCC_DDS = MAKEFOURCC('D', 'D', 'S', ' ');
    static const uint32_t FOURCC_DXT1 = MAKEFOURCC('D', 'X', 'T', '1');
    static const uint32_t FOURCC_DXT2 = MAKEFOURCC('D', 'X', 'T', '2');
    static const uint32_t FOURCC_DXT3 = MAKEFOURCC('D', 'X', 'T', '3');
    static const uint32_t FOURCC_DXT4 = MAKEFOURCC('D', 'X', 'T', '4');
    static const uint32_t FOURCC_DXT5 = MAKEFOURCC('D', 'X', 'T', '5');
    static const uint32_t FOURCC_RXGB = MAKEFOURCC('R', 'X', 'G', 'B');

    static const uint32_t DDSD_CAPS = 0x00000001l;
    static const uint32_t DDSD_PIXELFORMAT = 0x00001000l;
    static const uint32_t DDSD_WIDTH = 0x00000004l;
    static const uint32_t DDSD_HEIGHT = 0x00000002l;
    static const uint32_t DDSD_PITCH = 0x00000008l;

    static const uint32_t DDSCAPS_TEXTURE = 0x00001000l;
    static const uint32_t DDSCAPS2_VOLUME = 0x00200000l;
    static const uint32_t DDSCAPS2_CUBEMAP = 0x00000200l;

    static const uint32_t DDPF_RGB = 0x00000040l;
    static const uint32_t DDPF_FOURCC = 0x00000004l;
    static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;

    enum DDSType {
        DDS_A8R8G8B8 = 0,
        DDS_A1R5G5B5 = 1,
        DDS_A4R4G4B4 = 2,
        DDS_R8G8B8 = 3,
        DDS_R5G6B5 = 4,
        DDS_DXT1 = 5,
        DDS_DXT2 = 6,
        DDS_DXT3 = 7,
        DDS_DXT4 = 8,
        DDS_DXT5 = 9,
        DDS_RXGB = 10,
        DDS_UNKNOWN
    };


    struct DDSPixelFormat {
        uint32_t size;
        uint32_t flags;
        uint32_t fourcc;
        uint32_t bitcount;
        uint32_t rmask;
        uint32_t gmask;
        uint32_t bmask;
        uint32_t amask;
    };

    struct DDSCaps {
        uint32_t caps1;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
    };

    struct DDSHeader {
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitch;
        uint32_t depth;
        uint32_t mipmapcount;
        uint32_t reserved[11];
        DDSPixelFormat pf;
        DDSCaps caps;
        uint32_t notused;
    };

    static bool IsValid( const DDSHeader & header )
    {
        if( header.size != 124 ) {
            return false;
        }
        const uint32_t required = (DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT);
        if( (header.flags & required) != required ) {
            return false;
        }
        if( header.pf.size != 32 ) {
            return false;
        }
        if( !(header.caps.caps1 & DDSCAPS_TEXTURE) ) {
            return false;
        }
        return true;
    }

    bool
    readPixelFormat ( InputStream* in, DDSPixelFormat & pf )
    {
        const char* c;

        if (4 != in->read(c, 4, 4))
            return false;
        pf.size = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.flags = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.fourcc = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.bitcount = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.rmask = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.gmask = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.bmask = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        pf.amask = readLittleEndianUInt32(c);

        return true;
    }

    bool
    readCaps ( InputStream* in, DDSCaps & caps )
    {
        const char* c;

        if (4 != in->read(c, 4, 4))
            return false;
        caps.caps1 = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        caps.caps2 = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        caps.caps3 = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        caps.caps4 = readLittleEndianUInt32(c);

        return true;
    }

    bool
    readHeader(InputStream* in, DDSHeader& header)
    {
        const char *c;

        if (4 != in->read(c, 4, 4))
            return false;
        header.size = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.flags = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.height = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.width = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.pitch = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.depth = readLittleEndianUInt32(c);

        if (4 != in->read(c, 4, 4))
            return false;
        header.mipmapcount = readLittleEndianUInt32(c);

        for( int i = 0; i < 11; i++ ) {
            if (4 != in->read(c, 4, 4))
                return false;
            header.reserved[i] = readLittleEndianUInt32(c);
        }

        if (!readPixelFormat( in, header.pf))
            return false;


        if (!readCaps ( in, header.caps))
            return false;

        if (4 != in->read(c, 4, 4))
            return false;
        header.notused = readLittleEndianUInt32(c);

        return true;
    }
} // namespace


// AnalyzerFactory

void
DdsThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    volumeDepthField = reg.registerField( "http://strigi.sf.net/ontologies/homeless#ddsVolumeDepth");
    bitDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    mipmapCountField = reg.registerField("http://strigi.sf.net/ontologies/homeless#ddsMipmapCount");
    typeField = reg.registerField("http://strigi.sf.net/ontologies/homeless#ddsImageType");
    colorModeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");

    addField(widthField);
    addField(heightField);
    addField(volumeDepthField);
    addField(bitDepthField);
    addField(mipmapCountField);
    addField(typeField);
    addField(colorModeField);
    addField(compressionField);
}

// Analyzer
void
DdsThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
DdsThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    const char *c;

    //Remember: dds files are little-endian
    //read the beginning of the stream and make sure it looks ok

    if (4 != in->read(c, 4, 4)) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }

    uint32_t fourcc = readLittleEndianUInt32(c);
    if (fourcc != FOURCC_DDS) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }

    // Read image header.
    DDSHeader header;
    if (!readHeader (in, header)) {
        in->reset(0);
        return in;
    }

    // Check image file format.
    if(( in->read(c, 1, 1) != 1 ) || !IsValid( header ) ) {
        in->reset(0);
        return in;
    }

    // Set file info
    analysisResult->addValue( factory->widthField, header.width );
    analysisResult->addValue( factory->heightField, header.height );
    //FIXME: either get rid of this or replace with NIE equivalent
    //analysisResult->addValue( factory->mipmapCountField, header.mipmapcount );

    // Set file type.
    if( header.caps.caps2 & DDSCAPS2_CUBEMAP ) {
	//FIXME: either get rid of this or replace with NIE equivalent
        //analysisResult->addValue( factory->typeField, "Cube Map Texture");
    }
    else if( header.caps.caps2 & DDSCAPS2_VOLUME ) {
	//FIXME: either get rid of this or replace with NIE equivalent
        //analysisResult->addValue( factory->typeField, "Volume Texture");
        //analysisResult->addValue( factory->volumeDepthField, header.depth);
    }
    else {
	//FIXME: either get rid of this or replace with NIE equivalent
        //analysisResult->addValue( factory->typeField, "2D Texture");
    }

    // Set file color depth and compression.
    if( header.pf.flags & DDPF_RGB ) {
        analysisResult->addValue( factory->bitDepthField,header.pf.bitcount);
        analysisResult->addValue( factory->compressionField, "Uncompressed");
        if( header.pf.flags & DDPF_ALPHAPIXELS )
            analysisResult->addValue( factory->colorModeField, "RGB/Alpha");
        else
            analysisResult->addValue( factory->colorModeField, "RGB");
    }
    else if( header.pf.flags & DDPF_FOURCC ) {
        switch( header.pf.fourcc ) {
            case FOURCC_DXT1:
                analysisResult->addValue( factory->bitDepthField, 4);
                analysisResult->addValue( factory->compressionField, "DXT1");
                analysisResult->addValue( factory->colorModeField, "RGB");
                break;
            case FOURCC_DXT2:
                analysisResult->addValue( factory->bitDepthField, 16);
                analysisResult->addValue( factory->compressionField, "DXT2");
                analysisResult->addValue( factory->colorModeField, "RGB/Alpha");
                break;
            case FOURCC_DXT3:
                analysisResult->addValue( factory->bitDepthField, 16);
                analysisResult->addValue( factory->compressionField, "DXT3");
                analysisResult->addValue( factory->colorModeField, "RGB/Alpha");
                break;
            case FOURCC_DXT4:
                analysisResult->addValue( factory->bitDepthField, 16);
                analysisResult->addValue( factory->compressionField, "DXT4");
                analysisResult->addValue( factory->colorModeField, "RGB/Alpha");
                break;
            case FOURCC_DXT5:
                analysisResult->addValue( factory->bitDepthField, 16);
                analysisResult->addValue( factory->compressionField, "DXT5");
                analysisResult->addValue( factory->colorModeField, "RGB/Alpha");
                break;
            case FOURCC_RXGB:
                analysisResult->addValue( factory->bitDepthField, 16);
                analysisResult->addValue( factory->compressionField, "RXGB");
                analysisResult->addValue( factory->colorModeField, "RGB");
                break;
            default:
                analysisResult->addValue( factory->compressionField, "Unknown");
                break;
        }
    }
    else {
        analysisResult->addValue( factory->compressionField, "Unknown");
    }


    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
DdsThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new DdsThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
