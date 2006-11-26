/*
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odc_SP2003_ta/html/ODC_HowToWriteaFilter.asp

*/

#define STRIGI_IMPORT_API //todo: could also define this in cmake...
#include "jstreamsconfig.h"
#include "strigi_plugins.h"

#include "IFilterEndAnalyzer.h"
#include "streamindexer.h"
#include "indexable.h"
#include "indexwriter.h"
#include <filter.h>
#include <ntquery.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using namespace jstreams;

set<string> IFilterEndAnalyzer::extensions;

#ifndef ICONV_CONST
     //we try to guess whether the iconv function requires
     //a const char. We have no way of automatically figuring
     //this out if we did not use autoconf, so we guess based
     //on certain parameters:
     #ifdef _LIBICONV_H
          #define ICONV_CONST const
     #else
          #define ICONV_CONST
     #endif
#endif

IFilterEndAnalyzer::IFilterEndAnalyzer(){

#ifdef _LIBICONV_H
	if (sizeof(wchar_t) == 4) {
		converter = iconv_open("UTF-8//IGNORE", "UCS-4-INTERNAL");
	} if (sizeof(wchar_t) == 2) {
		converter = iconv_open("UTF-8//IGNORE", "UCS-2-INTERNAL");
#else
	if (sizeof(wchar_t) > 1) {
		converter = iconv_open("UTF-8//IGNORE", "WCHAR_T");
#endif
	} else {
		converter = iconv_open("UTF-8//IGNORE", "ASCII");
	}


		// check if the converter is valid
    if (converter == (iconv_t) -1) {
        return;
    }

	if ( extensions.size() == 0 ){
		HKEY pKey, kKey;
		if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "\\", 0, KEY_READ, &pKey) != ERROR_SUCCESS)
        {
			printf("failed open\n");
			RegCloseKey(pKey);
			return;
		}
		DWORD itr=0;
		TCHAR name[256];
		TCHAR key[256];
		while ( RegEnumKey(pKey, itr, name, 257) == ERROR_SUCCESS){
			if ( name[0] == '.' ){
				sprintf(key,"\\%s\\PersistentHandler",name);
				if (RegOpenKeyEx(HKEY_CLASSES_ROOT, key, 0, KEY_READ, &kKey) == ERROR_SUCCESS)
				{
					extensions.insert(name);
				}
				RegCloseKey(kKey);
			}
			itr++;
		}
		RegCloseKey(pKey);
	}
}
IFilterEndAnalyzer::~IFilterEndAnalyzer(){
	if (converter != (iconv_t) -1) {
        iconv_close(converter);
    }
}


void _cpycharToWide(wchar_t* d, const char* s, size_t len){
	size_t sLen = strlen(s);
    for ( uint32_t i=0;i<len&&i<sLen+1;i++ )
      d[i] = s[i];
}

bool
IFilterEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return true;
}
char
IFilterEndAnalyzer::analyze(jstreams::Indexable& idx, InputStream *in) {
	const string& filename = idx.getFileName();
	int p = filename.find_last_of('.');
	if ( p < 0 ||  extensions.find(filename.substr(p)) == extensions.end() ){
		return -1;
	}

	string filepath;
    bool fileisondisk = checkForFile(idx.getDepth(), filename);
    if (fileisondisk) {
        filepath = filename;
    } else {
		int p = filename.find_last_of(".");
		if ( p > 0 ){
			string ext = filename.substr(p).c_str();
			strlwr((char*)ext.c_str());
			p = ext.find_first_not_of("._abcdefghijklmnopqrstuvwxyz0123456789");
			if ( p >= 0 )
				filepath = writeToTempFile(in, "");
			else
				filepath = writeToTempFile(in, ext.c_str());
		}else
			filepath = writeToTempFile(in, "");

    }

	if ( filepath.length() > 0 ){

		IFilter* filter = NULL;
		void* pvfilter=NULL;

		wchar_t tmp[MAX_PATH];
		_cpycharToWide(tmp,filepath.c_str(),MAX_PATH);
		HRESULT hr = LoadIFilter(tmp,NULL,&pvfilter);
		if ( hr == S_OK ){
			filter = (IFilter*)pvfilter;

			ULONG __i=0;
			hr = filter->Init(IFILTER_INIT_APPLY_INDEX_ATTRIBUTES,0,NULL,&__i);
			if ( FAILED( hr ) ){
                if (!fileisondisk)
                    unlink(filepath.c_str());
				return -1;
			}

			ICONV_CONST char *inbuf;
			size_t inbytesleft;
			size_t outbytesleft=0;
			char *outbuf;
			wchar_t sbBuffer[1024];
			char asbBuffer[4096];


			STAT_CHUNK ps;
			hr = filter->GetChunk(&ps);
			while ( SUCCEEDED(hr) )
			{
				if (ps.flags == CHUNK_TEXT)
				{
					int resultText = 0;

					while ( resultText >= 0 )
					{
						ULONG sizeBuffer=1024;
						resultText = filter->GetText(&sizeBuffer, sbBuffer);
						if (sizeBuffer > 0 )
						{
							inbuf = (ICONV_CONST char *)sbBuffer;
							inbytesleft = sizeof(wchar_t)*sizeBuffer;
							outbuf = (char*)asbBuffer;
							outbytesleft = 4096;
							size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);

							if ( r >= 0 && ((size_t)r) != -1 ){
								idx.addText(asbBuffer,4096-outbytesleft);
							}else{
								switch (errno) {
								case EILSEQ: //invalid multibyte sequence
									printf("%s\n", "Invalid multibyte sequence.");
								    if (!fileisondisk){
										filter->Release();
								        printf("unklinking %s: %d\n", filepath.c_str(), unlink(filepath.c_str()));
                                    }
									return -1;
								case EINVAL: // last character is incomplete
									idx.addText(asbBuffer,4096-outbytesleft);
									break;
								case E2BIG: // output buffer is full
									break;
								default:
									char tmp[10];
									itoa(errno,tmp,10);
									this->error = "inputstreamreader error: ";
									this->error.append(tmp);
									//this->status = Error;
									printf("ifilterendanalyzer::error %d\n", errno);
									return -1;
								}
							}
						}
					}
				}else if ( ps.flags == CHUNK_VALUE ){
					PROPVARIANT *pVar;
					while ( SUCCEEDED( hr = filter->GetValue( &pVar ) ) )
					{
						//printf("propid: %d\nkind:%d\n",ps.attribute.psProperty.propid,ps.attribute.psProperty.ulKind);
						if ( ps.attribute.psProperty.propid == 2 &&
							 ps.attribute.psProperty.ulKind == 1 &&
							 pVar->vt == VT_LPWSTR ){


							inbuf = (ICONV_CONST char *)pVar->pwszVal;
							inbytesleft = sizeof(wchar_t)*wcslen(pVar->pwszVal);
							outbuf = (char*)asbBuffer;
							outbytesleft = 4096;
							size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
							if ( r >= 0 && ((size_t)r) != -1 ){
								idx.setField("title", string(asbBuffer,4096-outbytesleft) );

								//printf("title: %s\n", string(asbBuffer,4096-outbytesleft).c_str());
							}
						}
						PropVariantClear( pVar );
						CoTaskMemFree( pVar );
					}

				}else{
					printf("other flag %d\n",ps.flags);
				}
				hr = filter->GetChunk(&ps);
			}
			filter->Release();
			if (!fileisondisk)
                unlink(filepath.c_str());
			return 0;
		}


		DWORD dw = GetLastError();
		if ( dw != 0 ){
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );

			wprintf(L"%s\n", lpMsgBuf);
			LocalFree(lpMsgBuf);
		}
	}
	if (!fileisondisk && filepath.length()>0) {
        unlink(filepath.c_str());
    }
    return -1;
}

string
IFilterEndAnalyzer::writeToTempFile(jstreams::InputStream *in, const char* ext) const {
    string filepath = getenv("TMP");
	if ( filepath.length() == 0 )
		filepath = getenv("TEMP");
	filepath.append("\\strigiXXXXXX");
	{
		char* p = (char*)filepath.c_str();
		mktemp(p);
		filepath.append(ext);
	}
	int fd=open(filepath.c_str(),O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
    if (fd == -1) {
		unlink(filepath.c_str());
		fd=open(filepath.c_str(),O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
		if (fd == -1) {
			fprintf(stderr, "Error in making tmp name: %s\n", strerror(errno));
			unlink(filepath.c_str());
			return "";
		}
    }
    const char* b;
    int32_t nread = in->read(b, 1, 0);
    while (nread > 0) {
        do {
            ssize_t n = write(fd, b, nread);
            if (n == -1) {
                close(fd);
                unlink(filepath.c_str());
                return "";
            }
            nread -= n;
        } while (nread > 0);
        nread = in->read(b, 1, 0);
    }
    close(fd);
    return filepath;
}
bool
IFilterEndAnalyzer::checkForFile(int depth, const std::string& filename) {
    if (depth > 0) return false;
    struct stat s;
    if (stat(filename.c_str(), &s)) return false;
    return true;
}


long IFilterEndAnalyzer::initd = CoInitialize(NULL);


//define all the available analyzers in this plugin
STRIGI_END_PLUGINS_START
STRIGI_END_PLUGINS_REGISTER(IFilterEndAnalyzer)
STRIGI_END_PLUGINS_END
