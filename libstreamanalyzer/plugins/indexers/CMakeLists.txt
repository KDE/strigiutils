
if ( ENABLE_CLUCENE_NG AND CLUCENE1_FOUND )
    add_subdirectory(clucenengindexer)
elseif ( ENABLE_CLUCENE AND CLucene_FOUND )
    add_subdirectory(cluceneindexer)
else ( ENABLE_CLUCENE_NG AND CLUCENE1_FOUND )
    # don't build any clucene based indexer
endif ( ENABLE_CLUCENE_NG AND CLUCENE1_FOUND )

