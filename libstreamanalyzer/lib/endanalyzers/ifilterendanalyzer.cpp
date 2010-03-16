/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odc_SP2003_ta/html/ODC_HowToWriteaFilter.asp

*/

#define STRIGI_IMPORT_API //todo: could also define this in cmake...
#include <strigi/strigiconfig.h>

#include "IFilterEndAnalyzer.h"
#include "streamanalyzer.h"
#include <strigi/analysisresult.h>
#include "indexwriter.h"
#include <filter.h>
#include <ntquery.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using namespace Strigi;

set<string> IFilterEndAnalyzer::extensions;

std::string wchartoutf8(const wchar_t* p, const wchar_t* e);

IFilterEndAnalyzer::IFilterEndAnalyzer(){
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
}

void
_cpycharToWide(wchar_t* d, const char* s, size_t len){
    size_t sLen = strlen(s);
    for (uint32_t i=0; i<len && i<sLen+1; ++i) {
      d[i] = s[i];
    }
}

bool
IFilterEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return true;
}
signed char
IFilterEndAnalyzer::analyze(AnalysisResult& idx, InputStream *in) {
    const string& filename = idx.fileName();
    int p = filename.find_last_of('.');
    if (p < 0 ||  extensions.find(filename.substr(p)) == extensions.end()) {
        return -1;
    }

    string filepath;
    bool fileisondisk = checkForFile(idx.depth(), filename);
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

    if (filepath.length() > 0) {

        IFilter* filter = NULL;
        void* pvfilter=NULL;

        wchar_t tmp[MAX_PATH];
        _cpycharToWide(tmp,filepath.c_str(),MAX_PATH);
        HRESULT hr = LoadIFilter(tmp,NULL,&pvfilter);
        if (hr == S_OK) {
            filter = (IFilter*)pvfilter;

            ULONG __i=0;
            hr = filter->Init(IFILTER_INIT_APPLY_INDEX_ATTRIBUTES,0,NULL,&__i);
            if (FAILED( hr )) {
                if (!fileisondisk)
                    unlink(filepath.c_str());
                return -1;
            }

            const int sbBufferLen = 1024;
            wchar_t sbBuffer[sbBufferLen];

            STAT_CHUNK ps;
            hr = filter->GetChunk(&ps);
            while ( SUCCEEDED(hr) ) {
                if (ps.flags == CHUNK_TEXT) {
                    int resultText = 0;

                    while ( resultText >= 0 ) {
                        ULONG sizeBuffer=sbBufferLen;
                        resultText = filter->GetText(&sizeBuffer, sbBuffer);
                        if (sizeBuffer > 0 ) {
                            string str = wchartoutf8(sbBuffer,sbBuffer+sizeBuffer);
                            idx.addText(str.c_str(),str.length());
                        }
                    }
                } else if ( ps.flags == CHUNK_VALUE ) {
                    PROPVARIANT *pVar;
                    while ( SUCCEEDED( hr = filter->GetValue( &pVar ) ) ) {
                        //printf("propid: %d\nkind:%d\n",ps.attribute.psProperty.propid,ps.attribute.psProperty.ulKind);
                        if ( ps.attribute.psProperty.propid == 2 &&
                             ps.attribute.psProperty.ulKind == 1 &&
                             pVar->vt == VT_LPWSTR ) {

                            string str = wchartoutf8(pVar->pwszVal,pVar->pwszVal+wcslen(pVar->pwszVal));
                            idx.addValue("title", str );
                        }
                        PropVariantClear( pVar );
                        CoTaskMemFree( pVar );
                    }
                } else {
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
        if ( dw != 0 ) {
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
IFilterEndAnalyzer::writeToTempFile(InputStream *in, const char* ext) const {
    string filepath;
    if (getenv("TMP")) {
        filepath.assign(getenv("TMP"));
    } else if (getenv("TEMP")) {
        filepath.assign(getenv("TEMP"));
    }
    // if neither TMP nor TEMP were set, we use the current working directory
    filepath.append("\\strigiXXXXXX");
    {
        char* p = (char*)filepath.c_str();
        mktemp(p);
        filepath.append(ext);
    }
    int fd = open(filepath.c_str(),
        O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
    if (fd == -1) {
        unlink(filepath.c_str());
        fd = open(filepath.c_str(),
            O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
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
