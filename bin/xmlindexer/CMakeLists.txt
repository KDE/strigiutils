add_library(librdfindexer STATIC
        rdfindexer.cpp
        tagmapping.cpp
	rdfindexwriter.cpp
)
target_link_libraries(librdfindexer ${LIBSTREAMANALYZER_LIBRARIES} ${LIBSTREAMS_LIBRARIES})

add_library(libxmlindexer STATIC
        xmlindexer.cpp
        tagmapping.cpp
	xmlindexwriter.cpp
)
target_link_libraries(libxmlindexer ${LIBSTREAMANALYZER_LIBRARIES} ${LIBSTREAMS_LIBRARIES})

add_executable(cgixmlindexer
        tagmapping.cpp
	xmlindexwriter.cpp
	cgixmlindexer.cpp)
target_link_libraries(cgixmlindexer ${LIBSTREAMANALYZER_LIBRARIES} ${LIBSTREAMS_LIBRARIES})

#add_executable(peranalyzerxml peranalyzerxml.cpp xmlindexwriter.cpp
#    tagmapping.cpp)
#target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(perfieldxml perfieldxml.cpp xmlindexwriter.cpp
    tagmapping.cpp)
target_link_libraries(perfieldxml ${LIBSTREAMANALYZER_LIBRARIES} ${LIBSTREAMS_LIBRARIES})

add_executable(ontoprint ontoprint.cpp)
target_link_libraries(ontoprint ${LIBSTREAMANALYZER_LIBRARIES})

# register all tests based on the data in the testdata directory
#FILE(GLOB_RECURSE allfiles ../../testdata/analyzers/*/config)
FOREACH(file ${allfiles})
  GET_FILENAME_COMPONENT(testdir ${file} PATH)
  GET_FILENAME_COMPONENT(dir ${testdir} NAME)
  FILE(GLOB_RECURSE ofiles "${testdir}/*")
  FOREACH(ofile ${ofiles})
    STRING(REPLACE "/analyzers/${dir}/" "/data/" ifile ${ofile})
    IF(NOT ${ifile} MATCHES "/.svn/" AND NOT ${ifile} MATCHES "config$")
      STRING(REPLACE "*/" "" testname ${ofile})
      STRING(REPLACE ${testdir} "" testname ${ofile})
      ADD_TEST("${dir}${testname}" peranalyzerxml -c ${testdir}/config
        -r ${ofile} ${ifile})
    ENDIF(NOT ${ifile} MATCHES "/.svn/" AND NOT ${ifile} MATCHES "config$")
  ENDFOREACH(ofile ${ofiles})
ENDFOREACH(file ${allfiles})
