/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*               <bvanklinken@gmail.com>
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef NO_PREFIX_QUERY
#include "PrefixFilter.h"
#include "CLucene/util/BitSet.h"

CL_NS_USE(util)
CL_NS_USE(index)
CL_NS_DEF(search)



StrigiPrefixFilter::StrigiPrefixFilter( Term* prefix )
{
    this->prefix = _CL_POINTER(prefix);
}

StrigiPrefixFilter::~StrigiPrefixFilter()
{
    _CLDECDELETE(prefix);
}

StrigiPrefixFilter::StrigiPrefixFilter( const StrigiPrefixFilter& copy ) :
    prefix( _CL_POINTER(copy.prefix) )
{
}

Filter* StrigiPrefixFilter::clone() const {
    return _CLNEW StrigiPrefixFilter(*this );
}

TCHAR* StrigiPrefixFilter::toString()
{
    //Instantiate a stringbuffer buffer to store the readable version temporarily
    CL_NS(util)::StringBuffer buffer;
    //check if field equal to the field of prefix
    if( prefix->field() != NULL ) {
      //Append the field of prefix to the buffer
      buffer.append(prefix->field());
      //Append a colon
      buffer.append(_T(":") );
    }
    //Append the text of the prefix
    buffer.append(prefix->text());
    buffer.append(_T("*"));

    //Convert StringBuffer buffer to TCHAR block and return it
    return buffer.toString();
}

/** Returns a BitSet with true for documents which should be permitted in
search results, and false for those that should not. */
BitSet* StrigiPrefixFilter::bits( IndexReader* reader )
{
    BitSet* bts = _CLNEW BitSet( reader->maxDoc() );
    TermEnum* enumerator = reader->terms(prefix);
    TermDocs* docs = reader->termDocs();
    const TCHAR* prefixText = prefix->text();
    const TCHAR* prefixField = prefix->field();
    int32_t prefixLen = prefix->textLength();
    Term* lastTerm = NULL;

    try{
        do{
            lastTerm = enumerator->term(false);
            if (lastTerm != NULL && lastTerm->field() == prefixField ){
                //now see if term->text() starts with prefixText
                int32_t termLen = lastTerm->textLength();
                if ( prefixLen>termLen )
                    break; //the prefix is longer than the term, can't be matched

                //check for prefix match
                if ( _tcsncmp(lastTerm->text(),prefixText,prefixLen)!=0 )
                    break;

                docs->seek(enumerator);
                while (docs->next()) {
                  bts->set(docs->doc());
                }
            }
        }while(enumerator->next());
    } _CLFINALLY(
      docs->close();
      _CLDELETE(docs);
      enumerator->close();
      _CLDELETE(enumerator);
    )

    return bts;
}

CL_NS_END
#endif
