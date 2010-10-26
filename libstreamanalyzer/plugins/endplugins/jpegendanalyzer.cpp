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
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/stringstream.h>
#include <strigi/textutils.h>
#include <exiv2/exif.hpp>
#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/error.hpp>
#include <math.h>

#ifdef _MSC_VER
// at least MSVC2008 needs this define for defines like M_LN2
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#if defined(__sun) || defined(sun) || defined(_MSC_VER)
#define HUGE_VALF HUGE_VAL
#endif

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
    signed char analyze(AnalysisResult& idx, ::InputStream* in);

private:
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
    const RegisteredField* jpegQualityField;
    const RegisteredField* userCommentField;
    const RegisteredField* jpegProcessField;
    const RegisteredField* thumbnailField;

    const RegisteredField* typeField;

};

#define NEXIF "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"

const string commentFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
const string manufacturerFieldName(NEXIF "make");
const string modelFieldName(NEXIF "model");
const string creationDateFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
const string widthFieldName("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
const string heightFieldName("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
const string orientationFieldName(NEXIF "orientation");
const string colorModeFieldName("http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
const string flashUsedFieldName(NEXIF "flash");
const string focalLengthFieldName(NEXIF "focalLength");
const string _35mmEquivalentFieldName(NEXIF "focalLengthIn35mmFilm");
const string ccdWidthFieldName("http://freedesktop.org/standards/xesam/1.0/core#ccdWidth");
const string exposureTimeFieldName(NEXIF "exposureTime");
const string apertureFieldName(NEXIF "apertureValue");
const string focusDistFieldName("http://freedesktop.org/standards/xesam/1.0/core#focusDistance");
const string exposureBiasFieldName(NEXIF "exposureBiasValue");
const string whiteBalanceFieldName(NEXIF "whiteBalance");
const string meteringModeFieldName(NEXIF "meteringMode");
const string exposureFieldName(NEXIF "exposureProgram");
const string jpegQualityFieldName("http://freedesktop.org/standards/xesam/1.0/core#targetQuality");
const string userCommentFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
const string jpegProcessFieldName("http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
const string thumbnailFieldName("http://strigi.sf.net/ontologies/homeless#contentThumbnail");
const string ISOSpeedRatingsFieldName(NEXIF "isoSpeedRatings");

#undef NEXIF

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
JpegEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName);
    addField(commentField);

    exifFields["Exif.Image.DateTime"] = r.registerField(creationDateFieldName);
    exifFields["Exif.Image.Make"] = r.registerField(manufacturerFieldName);
    exifFields["Exif.Image.Model"] = r.registerField(modelFieldName);
    exifFields["Exif.Photo.PixelXDimension"] = r.registerField(widthFieldName);
    exifFields["Exif.Photo.PixelYDimension"] = r.registerField(heightFieldName);
    exifFields["Exif.Image.Orientation"] = r.registerField(orientationFieldName);
    exifFields["Exif.Photo.Flash"] = r.registerField(flashUsedFieldName);
    exifFields["Exif.Photo.FocalLength"] = r.registerField(focalLengthFieldName);
    exifFields["Exif.Photo.FocalLengthIn35mmFilm"] = r.registerField(_35mmEquivalentFieldName);
    exifFields["Exif.Photo.ExposureTime"] = r.registerField(exposureTimeFieldName);
    exifFields["Exif.Photo.ApertureValue"] = r.registerField(apertureFieldName);
    exifFields["Exif.Photo.ExposureBiasValue"] = r.registerField(exposureBiasFieldName);
    exifFields["Exif.Photo.WhiteBalance"] = r.registerField(whiteBalanceFieldName);
    exifFields["Exif.Photo.MeteringMode"] = r.registerField(meteringModeFieldName);
    exifFields["Exif.Photo.ISOSpeedRatings"] = r.registerField(ISOSpeedRatingsFieldName);

map<string, const RegisteredField*>::const_iterator i = exifFields.begin();
    for (; i != exifFields.end(); ++i) {
        addField(i->second);
    }

    colorModeField = r.registerField(colorModeFieldName);
    ccdWidthField = r.registerField(ccdWidthFieldName);
    focusDistField = r.registerField(focusDistFieldName);
    exposureField = r.registerField(exposureFieldName);
    jpegQualityField = r.registerField(jpegQualityFieldName);
    userCommentField = r.registerField(userCommentFieldName);
    jpegProcessField = r.registerField(jpegProcessFieldName);
    thumbnailField = r.registerField(thumbnailFieldName);
    typeField = r.typeField;

    addField(colorModeField);
    addField(ccdWidthField);
    addField(focusDistField);
    addField(exposureField);
    addField(jpegQualityField);
    addField(userCommentField);
    addField(jpegProcessField);
    addField(thumbnailField);
    addField(typeField);
}

bool
JpegEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char jpgmagic[]
        = {0xFF, 0xD8, 0xFF};
    return headersize >= 3 &&  memcmp(header, jpgmagic, 3) == 0;
}
namespace {

bool
fnumberToApertureValue(string& fnumber) {
    // Convert the F number to the aperture value
    // return true if the value in fnumber is changed to ApertureValue
    // F number = sqrt(pow(2, apex))
    char* end;
    double c = strtod(fnumber.c_str(), &end);
    if (c <= 0 || c == HUGE_VALF || *end == '\0') return false;
    double d = strtod(end + 1, &end);
    if (d <= 0 || d == HUGE_VALF) return false;
    c /= d;

    // do not use log2() or round() here to be cross-platform
    double apex = log(c*c) / M_LN2 * 65536;
    apex  = (apex < 0 ? ceil(apex - 0.5) : floor(apex + 0.5));
    std::ostringstream aperture;
    aperture << apex << "/65536";
    fnumber.assign(aperture.str());
    return true;
}

}

signed char
JpegEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream* in) {
    // parse the jpeg file now
    Exiv2::Image::AutoPtr img;
    bool ok = false;
    if (ar.depth() == 0) {
        try {
            // try to open the file directly: this is faster
            if (ar.path().compare(0, 5, "file:") == 0) {
                img = Exiv2::ImageFactory::open(ar.path().substr(5));
            } else {
                img = Exiv2::ImageFactory::open(ar.path());
            }
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
        while (nread > 0 && in->status() != Eof) {
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
        } catch(std::exception& e) {
            // trueg: here Exiv2::Error is not caught above, until the reason is found
            // I added this generic exception handling which should do no harm whatsoever
            m_error.assign(e.what());
            return -1;
        }
    }

    ar.addValue(factory->exifFields.find("Exif.Photo.PixelYDimension")->second, img->pixelHeight());
    ar.addValue(factory->exifFields.find("Exif.Photo.PixelXDimension")->second, img->pixelWidth());
    
    if (img->comment().length()) {
        ar.addValue(factory->commentField, img->comment());
    }

    const Exiv2::ExifData& exif = img->exifData();
    // if there's exif data, this is a photo, otherwise just an image
    if( exif.empty() ) {
      
        ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");
	return 0;
    }

    ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#Photo");

    for (Exiv2::ExifData::const_iterator i = exif.begin(); i != exif.end();++i){
        if (i->key() == "Exif.Photo.FNumber") {
            string aperture(i->toString());
            if (fnumberToApertureValue(aperture)) {
                ar.addValue(
                   factory->exifFields.find("Exif.Photo.ApertureValue")->second,
                   aperture);
            }
            continue;
        }
	if (i->key() == "Exif.Photo.PixelXDimension" || i->key() == "Exif.Photo.PixelYDimension") continue;
        map<string,const RegisteredField*>::const_iterator f
            = factory->exifFields.find(i->key());
        if (f != factory->exifFields.end() && f->second) {
            ar.addValue(f->second, i->toString());
//        } else {
//            cerr << i->key() << "\t" << i->value() << endl;
        }
    }
#if (EXIV2_TEST_VERSION(0,17,91))
    Exiv2::ExifThumbC thumb(exif);
    Exiv2::DataBuf thumbnail = thumb.copy();
#else
    Exiv2::DataBuf thumbnail = exif.copyThumbnail();
#endif
    data = (const char*)thumbnail.pData_;
    if (data) {
        StringInputStream thumbstream(data, (int32_t)thumbnail.size_, false);
        string thumbname("thumbnail");
#if (EXIV2_TEST_VERSION(0,17,91))
        ar.indexChild(thumbname + thumb.extension(), ar.mTime(),
            &thumbstream);
#else
        ar.indexChild(thumbname + exif.thumbnailExtension(), ar.mTime(),
            &thumbstream);
#endif
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
