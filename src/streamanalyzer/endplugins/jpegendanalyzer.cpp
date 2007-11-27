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
/*
 Include the strigi specific headers.
*/
#define STRIGI_IMPORT_API
#include "analyzerplugin.h"
#include "streamendanalyzer.h"
#include "analysisresult.h"
#include "fieldtypes.h"
#include "stringstream.h"
#include <exiv2/exif.hpp>
#include <exiv2/image.hpp>
#include <exiv2/error.hpp>

using namespace Strigi;
using namespace std;

/*
 Declare the factory.
*/
class JpegEndAnalyzerFactory;

/*
Define a class that inherits from StreamEndAnalyzer.
The only function we really need to implement is connectInputStream()
*/
class STRIGI_PLUGIN_API JpegEndAnalyzer : public StreamEndAnalyzer {
private:
    AnalysisResult* result;
    const JpegEndAnalyzerFactory* factory;

    //QDateTime parseDateTime(const QString& string);
public:
    JpegEndAnalyzer(const JpegEndAnalyzerFactory* f) :factory(f) {}
    ~JpegEndAnalyzer() {}
    const char* name() const {
        return "JpegEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(AnalysisResult& idx, ::InputStream* in);
};

/*
 Define a factory class the provides information about the fields that an
 analyzer can extract. This has a function similar to KFilePlugin::addItemInfo.
*/
class STRIGI_PLUGIN_API JpegEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class JpegEndAnalyzer;
private:
    /* This is why this class is a factory. */
    StreamEndAnalyzer* newInstance() const {
        return new JpegEndAnalyzer(this);
    }
    const char* name() const {
        return "JpegEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    /* define static fields that contain the field names. */
    static const string commentFieldName;
    static const string manufacturerFieldName;
    static const string modelFieldName;
    static const string creationDateFieldName;
    static const string widthFieldName;
    static const string heightFieldName;
    static const string orientationFieldName;
    static const string colorModeFieldName;
    static const string flashUsedFieldName;
    static const string focalLengthFieldName;
    static const string _35mmEquivalentFieldName;
    static const string ccdWidthFieldName;
    static const string exposureTimeFieldName;
    static const string apertureFieldName;
    static const string focusDistFieldName;
    static const string exposureBiasFieldName;
    static const string whiteBalanceFieldName;
    static const string meteringModeFieldName;
    static const string exposureFieldName;
    static const string isoEquivFieldName;
    static const string jpegQualityFieldName;
    static const string userCommentFieldName;
    static const string jpegProcessFieldName;
    static const string thumbnailFieldName;

    /* The RegisteredField instances are used to index specific fields quickly.
       We pass a pointer to the instance instead of a string.
    */
    map<string, const RegisteredField*> exifFields;
    const RegisteredField* commentField;
    const RegisteredField* manufacturerField;
    const RegisteredField* modelField;
    const RegisteredField* creationDateField;
    const RegisteredField* widthField;
    const RegisteredField* heightField;
    const RegisteredField* orientationField;
    const RegisteredField* colorModeField;
    const RegisteredField* flashUsedField;
    const RegisteredField* focalLengthField;
    const RegisteredField* _35mmEquivalentField;
    const RegisteredField* ccdWidthField;
    const RegisteredField* exposureTimeField;
    const RegisteredField* apertureField;
    const RegisteredField* focusDistField;
    const RegisteredField* exposureBiasField;
    const RegisteredField* whiteBalanceField;
    const RegisteredField* meteringModeField;
    const RegisteredField* exposureField;
    const RegisteredField* isoEquivField;
    const RegisteredField* jpegQualityField;
    const RegisteredField* userCommentField;
    const RegisteredField* jpegProcessField;
    const RegisteredField* thumbnailField;

    const RegisteredField* typeField;

};

const string JpegEndAnalyzerFactory::commentFieldName("http://freedesktop.org/standards/xesam/1.0/core#contentComment");
const string JpegEndAnalyzerFactory::manufacturerFieldName("http://freedesktop.org/standards/xesam/1.0/core#cameraManufacturer");
const string JpegEndAnalyzerFactory::modelFieldName("http://freedesktop.org/standards/xesam/1.0/core#cameraModel");
const string JpegEndAnalyzerFactory::creationDateFieldName("http://freedesktop.org/standards/xesam/1.0/core#contentCreated");
const string JpegEndAnalyzerFactory::widthFieldName("http://freedesktop.org/standards/xesam/1.0/core#width");
const string JpegEndAnalyzerFactory::heightFieldName("http://freedesktop.org/standards/xesam/1.0/core#height");
const string JpegEndAnalyzerFactory::orientationFieldName("http://freedesktop.org/standards/xesam/1.0/core#orientation");
const string JpegEndAnalyzerFactory::colorModeFieldName("http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
const string JpegEndAnalyzerFactory::flashUsedFieldName("http://freedesktop.org/standards/xesam/1.0/core#flashUsed");
const string JpegEndAnalyzerFactory::focalLengthFieldName("http://freedesktop.org/standards/xesam/1.0/core#focalLength");
const string JpegEndAnalyzerFactory::_35mmEquivalentFieldName("http://freedesktop.org/standards/xesam/1.0/core#35mmEquivalent");
const string JpegEndAnalyzerFactory::ccdWidthFieldName("http://freedesktop.org/standards/xesam/1.0/core#ccdWidth");
const string JpegEndAnalyzerFactory::exposureTimeFieldName("http://freedesktop.org/standards/xesam/1.0/core#exposureTime");
const string JpegEndAnalyzerFactory::apertureFieldName("http://freedesktop.org/standards/xesam/1.0/core#aperture");
const string JpegEndAnalyzerFactory::focusDistFieldName("http://freedesktop.org/standards/xesam/1.0/core#focusDistance");
const string JpegEndAnalyzerFactory::exposureBiasFieldName("http://freedesktop.org/standards/xesam/1.0/core#exposureBias");
const string JpegEndAnalyzerFactory::whiteBalanceFieldName("http://freedesktop.org/standards/xesam/1.0/core#whiteBalance");
const string JpegEndAnalyzerFactory::meteringModeFieldName("http://freedesktop.org/standards/xesam/1.0/core#meteringMode");
const string JpegEndAnalyzerFactory::exposureFieldName("http://freedesktop.org/standards/xesam/1.0/core#exposureProgram");
const string JpegEndAnalyzerFactory::isoEquivFieldName("http://freedesktop.org/standards/xesam/1.0/core#isoEquivalent");
const string JpegEndAnalyzerFactory::jpegQualityFieldName("http://freedesktop.org/standards/xesam/1.0/core#targetQuality");
const string JpegEndAnalyzerFactory::userCommentFieldName("http://freedesktop.org/standards/xesam/1.0/core#userComment");
const string JpegEndAnalyzerFactory::jpegProcessFieldName("http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
const string JpegEndAnalyzerFactory::thumbnailFieldName("content.thumbnail");

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
JpegEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName, FieldRegister::stringType,
        -1, 0);

    exifFields["Exif.Image.DateTime"] = r.registerField(creationDateFieldName);
    exifFields["Exif.Image.Make"] = r.registerField(manufacturerFieldName);
    exifFields["Exif.Image.Model"] = r.registerField(modelFieldName);
    exifFields["Exif.Photo.PixelXDimension"] = r.registerField(widthFieldName);
    exifFields["Exif.Photo.PixelYDimension"] = r.registerField(heightFieldName);
    exifFields["Exif.Image.Orientation"] = r.registerField(orientationFieldName,
        FieldRegister::stringType, -1 ,0);
    exifFields["Exif.Photo.Flash"] = r.registerField(flashUsedFieldName,
        FieldRegister::integerType, -1, 0);
    exifFields["Exif.Photo.FocalLength"] = r.registerField(focalLengthFieldName);
    exifFields["Exif.Photo.FocalLengthIn35mmFilm"] = r.registerField(
        _35mmEquivalentFieldName, FieldRegister::integerType, -1, 0);
    exifFields["Exif.Photo.ExposureTime"] =
        r.registerField(exposureTimeFieldName, FieldRegister::floatType, -1, 0);
    exifFields["Exif.Photo.ApertureValue"] =
        r.registerField(apertureFieldName, FieldRegister::floatType, -1, 0);
    exifFields["Exif.Photo.ExposureBiasValue"] =
        r.registerField(exposureBiasFieldName, FieldRegister::floatType, -1, 0);
    exifFields["Exif.Photo.WhiteBalance"] =
        r.registerField(whiteBalanceFieldName, FieldRegister::integerType,-1,0);
    exifFields["Exif.Photo.MeteringMode"] =
        r.registerField(meteringModeFieldName, FieldRegister::integerType,-1,0);


    colorModeField = r.registerField(colorModeFieldName);
    ccdWidthField = r.registerField(ccdWidthFieldName);
    focusDistField = r.registerField(focusDistFieldName);
    exposureField = r.registerField(exposureFieldName);
    isoEquivField = r.registerField(isoEquivFieldName);
    jpegQualityField = r.registerField(jpegQualityFieldName);
    userCommentField = r.registerField(userCommentFieldName);
    jpegProcessField = r.registerField(jpegProcessFieldName);
    thumbnailField = r.registerField(thumbnailFieldName);

    typeField = r.typeField;
}

bool
JpegEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char jpgmagic[]
        = {0xFF, 0xD8, 0xFF};
    return headersize >= 3 &&  memcmp(header, jpgmagic, 3) == 0;
}
char
JpegEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream* in) {
    // parse the jpeg file now
    Exiv2::Image::AutoPtr img;
    bool ok = false;
    if (ar.depth() == 0) {
        try {
            // try to open the file directly: this is faster
            img = Exiv2::ImageFactory::open(ar.path());
            img->readMetadata();
            ok = true;
        } catch (...) {
            // no problem yet, we can read from the stream
            cerr << "error reading " << ar.path() << endl;
        }
    }

    const char* data;
    if (!ok) {
        // read the entire input stream
        int32_t nread = in->read(data, 1, 0);
        while (nread > 0 && nread > 0 && in->status() != Eof) {
            in->reset(0);
            nread = in->read(data, 2*nread, 0);
        }
        in->reset(0);
        if (nread <= 0) {
            m_error.assign("no valid jpeg");
            return -1;
        }
    
        try {
            const Exiv2::byte* d = (const Exiv2::byte*)data;
            img = Exiv2::ImageFactory::open(d, nread);
            img->readMetadata();
        } catch (Exiv2::Error& e) {
            // even though this is the child class of Exiv2::Error, we seem to need
            // to catch it separately
            m_error.assign(e.what());
            return -1;
        } catch (Exiv2::AnyError& e) {
            m_error.assign(e.what());
            return -1;
        }
    }

    if (img->comment().length()) {
        ar.addValue(factory->commentField, img->comment());
    }

    const Exiv2::ExifData& exif = img->exifData();
    // if there's exif data, this is a photo, otherwise just an image
    if( exif.size() ) {
        ar.addValue(factory->typeField, "http://freedesktop.org/standards/xesam/1.0/core#Photo");
    } else {
        ar.addValue(factory->typeField, "http://freedesktop.org/standards/xesam/1.0/core#Image");
    }

    for (Exiv2::ExifData::const_iterator i = exif.begin(); i != exif.end();i++){
        map<string,const RegisteredField*>::const_iterator f
            = factory->exifFields.find(i->key());
        if (f != factory->exifFields.end() && f->second) {
            ar.addValue(f->second, i->toString());
//        } else {
//            cerr << i->key() << "\t" << i->value() << endl;
        }
    }

    Exiv2::DataBuf thumbnail = exif.copyThumbnail();
    data = (const char*)thumbnail.pData_;
    if (data) {
        StringInputStream thumbstream(data, thumbnail.size_, false);
        string thumbname("thumbnail");
        ar.indexChild(thumbname + exif.thumbnailExtension(), ar.mTime(),
            &thumbstream);
    }

/*

    ar.addValue(factory->colorModeField, ImageInfo.getIsColor()
        ?"Color" :"Black and white");

    int flashUsed = ImageInfo.getFlashUsed(); // -1, <set>
    if (flashUsed >= 0) {
	 string flash = "(unknown)";
         switch (flashUsed) {
         case 0: flash = "No";
             break;
         case 1:
         case 5:
         case 7:
             flash = "Fired";
             break;
         case 9:
         case 13:
         case 15:
             flash = "Fill Fired";
             break;
         case 16:
             flash = "Off";
             break;
         case 24:
             flash = "Auto Off";
             break;
         case 25:
         case 29:
         case 31:
             flash = "Auto Fired";
             break;
         case 32:
             flash = "Not Available";
             break;
         default:
             break;
        }
        ar.addValue(factory->flashUsedField, flash);
    }

    if (ImageInfo.getFocalLength()) {
        ar.addValue(factory->focalLengthField, ImageInfo.getFocalLength());

        if (ImageInfo.getCCDWidth()){
            ar.addValue(factory->_35mmEquivalentField,
                        (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5) );
	}
    }

    if (ImageInfo.getCCDWidth()) {
        ar.addValue(factory->ccdWidthField, ImageInfo.getCCDWidth());
    }

    if (ImageInfo.getExposureTime()) {
        tag = (const char*)
            QString().sprintf("%6.3f", ImageInfo.getExposureTime()).toUtf8();
        float exposureTime = ImageInfo.getExposureTime();
	if (exposureTime > 0 && exposureTime <= 0.5){
            tag += (const char*)
                QString().sprintf(" (1/%d)", (int)(0.5 + 1/exposureTime) ).toUtf8();
	}
        ar.addValue(factory->exposureTimeField, tag);
    }

    if (ImageInfo.getApertureFNumber()) {
        ar.addValue(factory->apertureField, ImageInfo.getApertureFNumber());
    }

    if (ImageInfo.getDistance()) {
        if (ImageInfo.getDistance() < 0) {
	    tag = "Infinite";
        } else {
	    tag = (const char*)QString()
                .sprintf("%5.2fm",(double)ImageInfo.getDistance()).toUtf8();
        }
        ar.addValue(factory->focusDistField, tag);
    }

    if (ImageInfo.getExposureBias()) {
        ar.addValue(factory->exposureBiasField, ImageInfo.getExposureBias());
    }

    if (ImageInfo.getWhitebalance() != -1) {
        switch(ImageInfo.getWhitebalance()) {
	case 0:
	    tag = "Unknown";
	    break;
	case 1:
	    tag = "Daylight";
	    break;
	case 2:
	    tag = "Fluorescent";
	    break;
	case 3:
	    //tag = "incandescent";
	    tag = "Tungsten";
	    break;
	case 17:
	    tag = "Standard light A";
	    break;
	case 18:
	    tag = "Standard light B";
	    break;
	case 19:
	    tag = "Standard light C";
	    break;
	case 20:
	    tag = "D55";
	    break;
	case 21:
	    tag = "D65";
	    break;
	case 22:
	    tag = "D75";
	    break;
	case 255:
	    tag = "Other";
	    break;
	default:
            //23 to 254 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->whiteBalanceField, tag);
    }

    if (ImageInfo.getMeteringMode() != -1) {
        switch(ImageInfo.getMeteringMode()) {
	case 0:
	    tag = "Unknown";
	    break;
	case 1:
	    tag = "Average";
	    break;
	case 2:
	    tag = "Center weighted average";
	    break;
	case 3:
	    tag = "Spot";
	    break;
	case 4:
	    tag = "MultiSpot";
	    break;
	case 5:
	    tag = "Pattern";
	    break;
	case 6:
	    tag = "Partial";
	    break;
	case 255:
	    tag = "Other";
	    break;
	default:
	    // 7 to 254 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->meteringModeField, tag);
    }

    if (ImageInfo.getExposureProgram()){
        switch(ImageInfo.getExposureProgram()) {
	case 0:
	    tag = "Not defined";
	    break;
	case 1:
	    tag = "Manual";
	    break;
	case 2:
	    tag = "Normal program";
	    break;
	case 3:
	    tag = "Aperture priority";
	    break;
	case 4:
	    tag = "Shutter priority";
	    break;
	case 5:
	    tag = "Creative program\n(biased toward fast shutter speed)";
	    break;
	case 6:
	    tag = "Action program\n(biased toward fast shutter speed)";
	    break;
	case 7:
	    tag = "Portrait mode\n(for closeup photos with the background out of focus)";
	    break;
	case 8:
	    tag = "Landscape mode\n(for landscape photos with the background in focus)";
	    break;
	default:
	    // 9 to 255 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->exposureField, tag);
    }

    if (ImageInfo.getISOequivalent()){
	ar.addValue(factory->isoEquivField, (int)ImageInfo.getISOequivalent());
    }

    if (ImageInfo.getCompressionLevel()){
	switch(ImageInfo.getCompressionLevel()) {
	case 1:
	    tag = "Basic";
            break;
	case 2:
	    tag = "Normal";
	    break;
        case 4:
	    tag = "Fine";
	    break;
	default:
	    tag = "Unknown";
	}
        ar.addValue(factory->jpegQualityField, tag);
    }

    tag = (const char*)ImageInfo.getUserComment().toUtf8();
    if (tag.length()) {
        ar.addValue(factory->commentField, tag);
    }

    int a;
    for (a = 0; ; a++){
        if (ProcessTable[a].Tag == ImageInfo.getProcess() || ProcessTable[a].Tag == 0) {
            ar.addValue(factory->jpegProcessField, ProcessTable[a].Desc);
            break;
        }
    }

    if (!ImageInfo.isNullThumbnail()) {
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << ImageInfo.getThumbnail();
        ar.addValue(factory->thumbnailField, ba.data(), ba.size());
    }*/
    return 0;
}
/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        list<StreamEndAnalyzerFactory*> af;
        af.push_back(new JpegEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
