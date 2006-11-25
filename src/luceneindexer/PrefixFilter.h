/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_PrefixFilter
#define _lucene_search_PrefixFilter
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#ifndef NO_PREFIX_QUERY

#include "CLucene/index/Term.h"
#include "CLucene/search/SearchHeader.h"


CL_NS_DEF(search) 

    class PrefixFilter: public Filter 
    {
    private:
    	CL_NS(index)::Term* prefix;
    protected:
    	PrefixFilter( const PrefixFilter& copy );
    	
    public:
    	PrefixFilter(CL_NS(index)::Term* prefix);
    	~PrefixFilter();
    
    	/** Returns a BitSet with true for documents which should be permitted in
    	search results, and false for those that should not. */
    	CL_NS(util)::BitSet* bits( CL_NS(index)::IndexReader* reader );
    	
    	Filter* clone() const;
    	TCHAR* toString();
    };
CL_NS_END
#endif
#endif
