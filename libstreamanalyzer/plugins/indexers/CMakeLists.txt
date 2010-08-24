
if ( ENABLE_CLUCENE_NG AND CLUCENE1_FOUND )
	add_subdirectory(clucenengindexer)
else ( ENABLE_CLUCENE AND CLucene_FOUND )
	add_subdirectory(cluceneindexer)
endif ( ENABLE_CLUCENE_NG AND CLUCENE1_FOUND )

